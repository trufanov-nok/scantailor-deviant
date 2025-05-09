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

#ifndef INCOMPLETETHUMBNAIL_H_
#define INCOMPLETETHUMBNAIL_H_

#include "ThumbnailBase.h"
#include "IntrusivePtr.h"
#include <QPainterPath>
#include <memory>

class AbstractThumbnailMaker;
class ThumbnailPixmapCache;
class QSizeF;
class QRectF;
class QString;

/**
 * \brief A thumbnail represeting a page not completely processed.
 *
 * Suppose you have switched to the Deskew filter without running all images
 * through the previous filters.  The thumbnail view of the Deskew filter
 * is supposed to show already split and deskewed pages.  However, because
 * the previous filters were not applied to all pages, we can't show them
 * split and deskewed.  In that case, we show an image the best we can
 * (maybe split but not deskewed, maybe unsplit).  Also we draw a translucent
 * question mark over that image to indicate it's not shown the way it should.
 * This class implements drawing of such thumbnails with question marks.
 */
class IncompleteThumbnail : public ThumbnailBase
{
public:
    IncompleteThumbnail(
        IntrusivePtr<ThumbnailPixmapCache> const& thumbnail_cache,
        std::unique_ptr<AbstractThumbnailMaker> thumbnail_maker,
        QSizeF const& max_size, ImageId const& image_id, QString const& version,
        ImageTransformation const& image_xform);

    virtual ~IncompleteThumbnail();

    static void drawQuestionMark(QPainter& painter, QRectF const& bounding_rect);
protected:
    virtual void paintOverImage(
        QPainter& painter, QTransform const& image_to_display,
        QTransform const& thumb_to_display);
private:
    static QPainterPath m_sCachedPath;
};

#endif
