/*
    OpenSR - opensource multi-genre game based upon "Space Rangers 2: Dominators"
    Copyright (C) 2013 Kosyak <ObKo@mail.ru>

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

#include <boost/python.hpp>
#include <OpenSR/Sprite.h>

#include "OpenSR/python/SpriteWrap.h"

namespace Rangers
{
namespace Python
{
struct SpriteWrap : Sprite, boost::python::wrapper<Sprite>
{
    SpriteWrap(): Sprite()
    {
    }

    SpriteWrap(boost::shared_ptr<Texture> texture)
        : Sprite(texture)
    {
    }
    SpriteWrap(const std::wstring& texture)
        : Sprite(texture)
    {
    }

    SpriteWrap(const TextureRegion& region)
        : Sprite(region)
    {
    }

    SpriteWrap(const TextureRegionDescriptor& region)
        : Sprite(region)
    {
    }
    RANGERS_PYTHON_WRAP_SPRITE(Sprite)
};

void exportSprite()
{
    using namespace boost::python;
    enum_<TextureScaling>("TextureScaling")
    .value("TEXTURE_NO", TEXTURE_NO)
    .value("TEXTURE_NORMAL", TEXTURE_NORMAL)
    .value("TEXTURE_KEEPASPECT", TEXTURE_KEEPASPECT)
    .value("TEXTURE_KEEPASPECT_EXPANDING", TEXTURE_KEEPASPECT_EXPANDING)
    .value("TEXTURE_TILE_X", TEXTURE_TILE_X)
    .value("TEXTURE_TILE_Y", TEXTURE_TILE_Y)
    .value("TEXTURE_TILE", TEXTURE_TILE);

    enum_<SpriteXOrigin>("SpriteXOrigin")
    .value("POSITION_X_LEFT", POSITION_X_LEFT)
    .value("POSITION_X_RIGHT", POSITION_X_RIGHT)
    .value("POSITION_X_CENTER", POSITION_X_CENTER);

    enum_<SpriteYOrigin>("SpriteYOrigin")
    .value("POSITION_Y_TOP", POSITION_Y_TOP)
    .value("POSITION_Y_BOTTOM", POSITION_Y_BOTTOM)
    .value("POSITION_Y_CENTER", POSITION_Y_CENTER);

    class_<SpriteWrap, bases<Object>, boost::shared_ptr<SpriteWrap>, boost::noncopyable> c("Sprite", init<const std::wstring&>());
    c.def(init<const TextureRegion&>())
    .def(init<const TextureRegionDescriptor&>())
    .def(init<>())
    .def("region", &Sprite::region)
    .def("setOrigin", &Sprite::setOrigin)
    .def("setGeometry", &Sprite::setGeometry)
    .def("setWidth", &Sprite::setWidth)
    .def("setHeight", &Sprite::setHeight)
    .def("setTextureScaling", &Sprite::setTextureScaling)
    .def("setTexture", &Sprite::setTexture)
    .def("width", &Sprite::width)
    .def("height", &Sprite::height);
    RANGERS_PYTHON_WRAP_SPRITE_DEF(Sprite, SpriteWrap, c);
    register_ptr_to_python<boost::shared_ptr<Sprite> >();
}
}
}
