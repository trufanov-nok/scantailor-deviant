/*
    Scan Tailor - Interactive post-processing tool for scanned pages.
    Copyright (C)  Joseph Artsimovich <joseph.artsimovich@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "Settings.h"
#include "Params.h"
#include "PictureLayerProperty.h"
#include "FillColorProperty.h"
#include "RelinkablePath.h"
#include "AbstractRelinker.h"
#include "../../Utils.h"
#include <Qt>
#include <QColor>
#include <QMutexLocker>
#include <tiff.h>
#include <QResource>
#include "settings/ini_keys.h"
#include <QRegularExpression>

namespace output
{

Settings::Settings()
    :   m_defaultPictureZoneProps(initialPictureZoneProps()),
        m_defaultFillZoneProps(initialFillZoneProps()),
        m_compression(COMPRESSION_LZW)
{
}

Settings::~Settings()
{
}

void
Settings::clear()
{
    QMutexLocker const locker(&m_mutex);

    initialPictureZoneProps().swap(m_defaultPictureZoneProps);
    initialFillZoneProps().swap(m_defaultFillZoneProps);
    m_perPageParams.clear();
    m_perPageOutputParams.clear();
    m_perPagePictureZones.clear();
    m_perPageFillZones.clear();
}

void
Settings::performRelinking(AbstractRelinker const& relinker)
{
    QMutexLocker const locker(&m_mutex);

    PerPageParams new_params;
    PerPageOutputParams new_output_params;
    PerPageZones new_picture_zones;
    PerPageZones new_fill_zones;

    for (PerPageParams::value_type const& kv : m_perPageParams) {
        RelinkablePath const old_path(kv.first.imageId().filePath(), RelinkablePath::File);
        PageId new_page_id(kv.first);
        new_page_id.imageId().setFilePath(relinker.substitutionPathFor(old_path));
        new_params.insert(PerPageParams::value_type(new_page_id, kv.second));
    }

    for (PerPageOutputParams::value_type const& kv : m_perPageOutputParams) {
        RelinkablePath const old_path(kv.first.imageId().filePath(), RelinkablePath::File);
        PageId new_page_id(kv.first);
        new_page_id.imageId().setFilePath(relinker.substitutionPathFor(old_path));
        new_output_params.insert(PerPageOutputParams::value_type(new_page_id, kv.second));
    }

    for (PerPageZones::value_type const& kv : m_perPagePictureZones) {
        RelinkablePath const old_path(kv.first.imageId().filePath(), RelinkablePath::File);
        PageId new_page_id(kv.first);
        new_page_id.imageId().setFilePath(relinker.substitutionPathFor(old_path));
        new_picture_zones.insert(PerPageZones::value_type(new_page_id, kv.second));
    }

    for (PerPageZones::value_type const& kv : m_perPageFillZones) {
        RelinkablePath const old_path(kv.first.imageId().filePath(), RelinkablePath::File);
        PageId new_page_id(kv.first);
        new_page_id.imageId().setFilePath(relinker.substitutionPathFor(old_path));
        new_fill_zones.insert(PerPageZones::value_type(new_page_id, kv.second));
    }

    m_perPageParams.swap(new_params);
    m_perPageOutputParams.swap(new_output_params);
    m_perPagePictureZones.swap(new_picture_zones);
    m_perPageFillZones.swap(new_fill_zones);
}

Params
Settings::getParams(PageId const& page_id) const
{
    QMutexLocker const locker(&m_mutex);

    PerPageParams::const_iterator const it(m_perPageParams.find(page_id));
    if (it != m_perPageParams.end()) {
        return it->second;
    } else {
        return Params();
    }
}

void
Settings::setParams(PageId const& page_id, Params const& params)
{
    QMutexLocker const locker(&m_mutex);
    Utils::mapSetValue(m_perPageParams, page_id, params);
}

void
Settings::setColorParams(PageId const& page_id, ColorParams const& prms, ColorParamsApplyFilter const& filter)
{
    QMutexLocker const locker(&m_mutex);

    PerPageParams::iterator const it(m_perPageParams.lower_bound(page_id));
    if (it == m_perPageParams.end() || m_perPageParams.key_comp()(page_id, it->first)) {
        Params params;
        params.setColorParams(prms, filter);
        m_perPageParams.insert(it, PerPageParams::value_type(page_id, params));
    } else {
        ColorParams::ColorMode old_mode = it->second.colorParams().colorMode();
        if (old_mode == ColorParams::MIXED && prms.colorMode() != old_mode) {
            //clearPictureAutoZonesForPage(page_id);
            PerPageZones::iterator const itz(m_perPagePictureZones.find(page_id));
            if (itz != m_perPagePictureZones.end()) {
                ZoneSet zones = itz->second;
                zones.remove_auto_zones();
                itz->second = zones;
            }
        }
        it->second.setColorParams(prms, filter);
    }
}

static void copyBwOptions(BlackWhiteOptions *dst, BlackWhiteOptions const& src, std::vector<ThresholdFilter> const& thresholds, bool set_foreground)
{
    dst->setThresholdMethod(src.thresholdMethod());

    for (ThresholdFilter const threshold : thresholds)
    {
        switch (threshold)
        {
        case OTSU:
            dst->setThresholdOtsuAdjustment(src.thresholdOtsuAdjustment());
            break;
        case SAUVOLA:
            dst->setThresholdSauvolaAdjustment(src.thresholdSauvolaAdjustment());
            dst->setThresholdSauvolaWindowSize(src.thresholdSauvolaWindowSize());
            dst->setThresholdSauvolaCoef(src.thresholdSauvolaCoef());
            break;
        case WOLF:
            dst->setThresholdWolfAdjustment(src.thresholdWolfAdjustment());
            dst->setThresholdWolfWindowSize(src.thresholdWolfWindowSize());
            dst->setThresholdWolfCoef(src.thresholdWolfCoef());
            break;
        case GATOS:
            dst->setThresholdGatosAdjustment(src.thresholdGatosAdjustment());
            dst->setThresholdGatosWindowSize(src.thresholdGatosWindowSize());
            dst->setThresholdGatosCoef(src.thresholdGatosCoef());
            dst->setThresholdGatosScale(src.thresholdGatosScale());
            break;
        default:
            assert(!"Unreachable");
            break;
        }
    }

    if (set_foreground)
    {
        dst->setThresholdForegroundAdjustment(src.thresholdForegroundAdjustment());
    }
}

void
Settings::setColorParams(PageId const& page_id, ColorParams const& prms, std::vector<ThresholdFilter> const& thresholds, bool set_foreground)
{
    QMutexLocker const locker(&m_mutex);

    PerPageParams::iterator const it(m_perPageParams.lower_bound(page_id));
    if (it == m_perPageParams.end() || m_perPageParams.key_comp()(page_id, it->first)) {
        Params params;
        BlackWhiteOptions& bw_options = params.colorParams().blackWhiteOptions();
        BlackWhiteOptions const& bw_opt = prms.blackWhiteOptions();

        copyBwOptions(&bw_options, bw_opt, thresholds, set_foreground);

        m_perPageParams.insert(it, PerPageParams::value_type(page_id, params));
    }
    else {
        ColorParams::ColorMode old_mode = it->second.colorParams().colorMode();
        if (old_mode == ColorParams::MIXED && prms.colorMode() != old_mode) {
            //clearPictureAutoZonesForPage(page_id);
            PerPageZones::iterator const itz(m_perPagePictureZones.find(page_id));
            if (itz != m_perPagePictureZones.end()) {
                ZoneSet zones = itz->second;
                zones.remove_auto_zones();
                itz->second = zones;
            }
        }
        
        BlackWhiteOptions& bw_options = it->second.colorParams().blackWhiteOptions();
        BlackWhiteOptions const& bw_opt = prms.blackWhiteOptions();

        copyBwOptions(&bw_options, bw_opt, thresholds, set_foreground);
    }
}

void
Settings::setDpi(PageId const& page_id, Dpi const& dpi)
{
    QMutexLocker const locker(&m_mutex);

    PerPageParams::iterator const it(m_perPageParams.lower_bound(page_id));
    if (it == m_perPageParams.end() || m_perPageParams.key_comp()(page_id, it->first)) {
        Params params;
        params.setOutputDpi(dpi);
        m_perPageParams.insert(it, PerPageParams::value_type(page_id, params));
    } else {
        it->second.setOutputDpi(dpi);
    }
}


void
Settings::setDespeckleLevel(PageId const& page_id, DespeckleLevel level)
{
    QMutexLocker const locker(&m_mutex);

    PerPageParams::iterator const it(m_perPageParams.lower_bound(page_id));
    if (it == m_perPageParams.end() || m_perPageParams.key_comp()(page_id, it->first)) {
        Params params;
        params.setDespeckleLevel(level);
        m_perPageParams.insert(it, PerPageParams::value_type(page_id, params));
    } else {
        it->second.setDespeckleLevel(level);
    }
}

std::unique_ptr<OutputParams>
Settings::getOutputParams(PageId const& page_id) const
{
    QMutexLocker const locker(&m_mutex);

    PerPageOutputParams::const_iterator const it(m_perPageOutputParams.find(page_id));
    if (it != m_perPageOutputParams.end()) {
        return std::unique_ptr<OutputParams>(new OutputParams(it->second));
    } else {
        return std::unique_ptr<OutputParams>();
    }
}

void
Settings::removeOutputParams(PageId const& page_id)
{
    QMutexLocker const locker(&m_mutex);
    m_perPageOutputParams.erase(page_id);
}

void
Settings::setOutputParams(PageId const& page_id, OutputParams const& params)
{
    QMutexLocker const locker(&m_mutex);
    Utils::mapSetValue(m_perPageOutputParams, page_id, params);
}

ZoneSet
Settings::pictureZonesForPage(PageId const& page_id) const
{
    QMutexLocker const locker(&m_mutex);

    PerPageZones::const_iterator const it(m_perPagePictureZones.find(page_id));
    if (it != m_perPagePictureZones.end()) {
        return it->second;
    } else {
        return ZoneSet();
    }
}

ZoneSet
Settings::fillZonesForPage(PageId const& page_id) const
{
    QMutexLocker const locker(&m_mutex);

    PerPageZones::const_iterator const it(m_perPageFillZones.find(page_id));
    if (it != m_perPageFillZones.end()) {
        return it->second;
    } else {
        return ZoneSet();
    }
}

void
Settings::setPictureZones(PageId const& page_id, ZoneSet const& zones)
{
    QMutexLocker const locker(&m_mutex);
    Utils::mapSetValue(m_perPagePictureZones, page_id, zones);
}

void
Settings::setFillZones(PageId const& page_id, ZoneSet const& zones)
{
    QMutexLocker const locker(&m_mutex);
    Utils::mapSetValue(m_perPageFillZones, page_id, zones);
}

PropertySet
Settings::defaultPictureZoneProperties() const
{
    QMutexLocker const locker(&m_mutex);
    return m_defaultPictureZoneProps;
}

PropertySet
Settings::defaultFillZoneProperties() const
{
    QMutexLocker const locker(&m_mutex);
    return m_defaultFillZoneProps;
}

void
Settings::setDefaultPictureZoneProperties(PropertySet const& props)
{
    QMutexLocker const locker(&m_mutex);
    m_defaultPictureZoneProps = props;
}

void
Settings::setDefaultFillZoneProperties(PropertySet const& props)
{
    QMutexLocker const locker(&m_mutex);
    m_defaultFillZoneProps = props;
}

PropertySet
Settings::initialPictureZoneProps()
{
    PropertySet props;
    props.locateOrCreate<PictureLayerProperty>()->setLayer(PictureLayerProperty::PAINTER2);
    return props;
}

PropertySet
Settings::initialFillZoneProps()
{
    PropertySet props;
    props.locateOrCreate<FillColorProperty>()->setColor(Qt::white);
    return props;
}

} // namespace output
