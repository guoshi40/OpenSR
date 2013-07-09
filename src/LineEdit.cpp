/*
    OpenSR - opensource multi-genre game based upon "Space Rangers 2: Dominators"
    Copyright (C) 2011 - 2013 Kosyak <ObKo@mail.ru>

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

#include "OpenSR/LineEdit.h"
#include <SDL.h>
#include <boost/variant/get.hpp>

#include "OpenSR/Engine.h"
#include "OpenSR/Font.h"
#include "OpenSR/ActionListener.h"
#include "OpenSR/Action.h"
#include "OpenSR/ResourceManager.h"
#include "OpenSR/NinePatch.h"

#include "OpenSR/private/LineEdit_p.h"

namespace Rangers
{

void LineEditPrivate::LineEditListener::actionPerformed(const Action &action)
{
    boost::shared_ptr<LineEdit> q = boost::static_pointer_cast<LineEdit>(action.source());
    LineEditPrivate *d = q->d_func();

    switch (action.type())
    {
    case Action::MOUSE_CLICK:
        d->position = d->label->font()->maxChars(d->text.begin() + d->stringOffset, d->text.end(),
                      d->mousePosition.x/* - style.contentRect.x*/);
        d->updateText();
        Engine::instance().focusWidget(q);
        break;
    case Action::KEY_PRESSED:
        d->keyPressed(boost::get<SDL_Keysym>(action.argument()));
        break;
    case Action::TEXT_INPUT:
        d->textAdded(boost::get<std::wstring>(action.argument()));
        break;
    }
}

LineEditPrivate::LineEditPrivate()
{
    cursorVertices = 0;
    cursorBuffer = 0;
    position = 0;
    cursorVisible = false;
    cursorTime = 0;
    stringOffset = 0;
    lineEditListener = boost::shared_ptr<LineEditListener>(new LineEditListener());
}

void LineEdit::draw() const
{
    RANGERS_D(const LineEdit);
    if (!prepareDraw())
        return;

    if (d->background)
        d->background->draw();

    d->label->draw();

    if (isFocused() && d->cursorVisible && d->cursorBuffer)
    {
        glBindTexture(GL_TEXTURE_2D, 0);
        glColor3ub((d->style.color >> 24) & 0xff, (d->style.color >> 16) & 0xff, (d->style.color >> 8) & 0xff);

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_ARRAY_BUFFER);

        glBindBuffer(GL_ARRAY_BUFFER, d->cursorBuffer);

        glVertexPointer(2, GL_FLOAT, sizeof(Vertex), 0);
        glLineWidth(1);

        glDrawArrays(GL_LINES, 0, 2);

        glDisableClientState(GL_ARRAY_BUFFER);
        glDisableClientState(GL_VERTEX_ARRAY);
    }
    endDraw();
}

LineEdit::LineEdit(float w, float h, boost::shared_ptr< Font > font):
    Widget(*(new LineEditPrivate()))
{
    RANGERS_D(LineEdit);
    if (!font)
        font = Engine::instance().coreFont();

    d->label = boost::shared_ptr<Label>(new Label(L"", font));
    addChild(d->label);

    d->height = h > font->size() + 4 ? h : font->size() + 4;
    d->width = w;
    d->style.color = 0xffffffff;
    d->init();
}

LineEdit::LineEdit(): Widget(*(new LineEditPrivate()))
{
}

LineEdit::LineEdit(const LineEditStyle& style): Widget(*(new LineEditPrivate()))
{
    RANGERS_D(LineEdit);
    d->style = style;
    d->init();
}

LineEdit::LineEdit(LineEditPrivate &p): Widget(p)
{
}

void LineEditPrivate::init()
{
    RANGERS_Q(LineEdit);
    if (style.background.type == ResourceDescriptor::NINEPATCH)
    {
        background = boost::shared_ptr<Sprite>(new NinePatch(boost::get<NinePatchDescriptor>(style.background.resource)));
        q->addChild(background);
    }
    else if (style.background.type == ResourceDescriptor::SPRITE)
    {
        background = boost::shared_ptr<Sprite>(new Sprite(boost::get<TextureRegionDescriptor>(style.background.resource)));
        q->addChild(background);
    }
    if ((style.font.path != L"") && (style.font.size > 0))
    {
        label = boost::shared_ptr<Label>(new Label(text, ResourceManager::instance().loadFont(style.font.path, style.font.size)));
        label->setColor(((style.color >> 24) & 0xff) / 255.0f, ((style.color >> 16) & 0xff) / 255.0f, ((style.color >> 8) & 0xff) / 255.0f, ((style.color) & 0xff) / 255.0f);
        q->addChild(label);
    }
    else
    {
        if (!label)
        {
            label = boost::shared_ptr<Label>(new Label());
            q->addChild(label);
        }
    }
    if (style.contentRect.valid() && label->font() && background)
    {
        height = std::max(background->normalHeight() - style.contentRect.height + label->font()->size(), background->normalHeight());
        width = std::max(background->normalWidth(), style.contentRect.width);
    }
    else if (label->font() && background)
    {
        height = std::max(background->normalHeight(), (float)label->font()->size());
        width = background->normalWidth();
    }
    else if (style.contentRect.valid() && label->font())
    {
        height = std::max(style.contentRect.height, (float)label->font()->size());
        width = style.contentRect.width;
    }
    label->setOrigin(POSITION_X_LEFT, POSITION_Y_TOP);
    position = 0;
    cursorTime = 0;
    cursorVisible = false;
    cursorBuffer = 0;
    cursorVertices = 0;
    stringOffset = 0;
    q->addListener(lineEditListener);
    q->markToUpdate();
}

void LineEdit::mouseMove(const Vector &p)
{
    RANGERS_D(LineEdit);
    d->mousePosition = p;
}

void LineEditPrivate::updateText()
{
    RANGERS_Q(LineEdit);
    if (!label->font())
        return;
    q->lock();

    Rect realContentRect;
    if (style.contentRect.valid() && background)
    {
        realContentRect.x = style.contentRect.x;
        realContentRect.y = style.contentRect.y;
        realContentRect.width = (width - background->normalWidth() + style.contentRect.width);
        realContentRect.height = (height - background->normalHeight() + style.contentRect.height);
    }
    else
    {
        realContentRect.x = 0;
        realContentRect.y = 0;
        realContentRect.width = width;
        realContentRect.height = height;
    }

    int maxChars;
    if (position <= stringOffset)
    {
        stringOffset = position > 0 ? position - 1 : 0;
    }
    else
    {
        std::wstring::iterator start = text.begin() + stringOffset;
        std::wstring::iterator end = text.begin() + position;
        while ((maxChars = label->font()->maxChars(start, end, width)) < end - start)
        {
            stringOffset = (end - maxChars) - text.begin();
            start = text.begin() + stringOffset;
        }
    }
    maxChars = label->font()->maxChars(text.begin() + stringOffset, text.end(), realContentRect.width);
    label->setText(text.substr(stringOffset, maxChars));
    q->markToUpdate();
    q->unlock();
}

void LineEdit::processMain()
{
    RANGERS_D(LineEdit);

    if (d->label->needUpdate())
        d->label->processMain();

    lock();

    if (d->background)
    {
        d->background->setGeometry(d->width, d->height);
    }

    int cursorPosition = 0;
    if (d->label->font())
        cursorPosition = d->label->font()->calculateStringWidth(d->text.begin() + d->stringOffset, d->text.begin() + d->position);

    if (!d->cursorBuffer)
    {
        d->cursorVertices = new Vertex[2];
        glGenBuffers(1, &d->cursorBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, d->cursorBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 2, d->cursorVertices, GL_DYNAMIC_DRAW);
        delete[] d->cursorVertices;
    }

    glBindBuffer(GL_ARRAY_BUFFER, d->cursorBuffer);
    d->cursorVertices = (Vertex *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

    if (!d->style.contentRect.valid())
    {
        d->label->setPosition(0, 0);
        d->cursorVertices[0].x = cursorPosition - 0.5f;
        d->cursorVertices[0].y = d->label->height();
        d->cursorVertices[1].x = cursorPosition - 0.5f;
        d->cursorVertices[1].y = 0;
    }
    else
    {
        d->label->setPosition(d->style.contentRect.x, d->style.contentRect.y);
        d->cursorVertices[0].x = d->style.contentRect.x + cursorPosition - 0.5f;
        d->cursorVertices[0].y = d->label->height();
        d->cursorVertices[1].x = d->style.contentRect.x + cursorPosition - 0.5f;
        d->cursorVertices[1].y = d->style.contentRect.y;
    }

    glUnmapBuffer(GL_ARRAY_BUFFER);

    unlock();
    Widget::processMain();
}

void LineEdit::processLogic(int dt)
{
    lock();
    RANGERS_D(LineEdit);
    d->cursorTime += dt;
    if (d->cursorTime > 500)
    {
        d->cursorVisible = !d->cursorVisible;
        d->cursorTime = 0;
    }
    unlock();
}


void LineEditPrivate::keyPressed(const SDL_Keysym& key)
{
    RANGERS_Q(LineEdit);
    q->lock();

    if (key.sym == SDLK_RETURN);
    else if (key.sym == SDLK_LEFT)
        position--;
    else if (key.sym == SDLK_RIGHT)
        position++;
    else if (key.sym == SDLK_ESCAPE)
        text.clear();
    else if (key.sym == SDLK_BACKSPACE)
    {
        if (text.size())
            if (position > 0)
                text.erase(position - 1, 1);
        position--;
    }
    else if (key.sym == SDLK_DELETE)
    {
        if (text.size())
            if (position < text.length())
                text.erase(position, 1);
    }
    if (position < 0)
        position = 0;
    if (position > text.length())
        position = text.length();
    //label.setText(text);
    updateText();
    q->markToUpdate();

    q->unlock();
}

void LineEditPrivate::textAdded(const std::wstring& str)
{
    RANGERS_Q(LineEdit);
    q->lock();

    //FIXME: Handle "bad" chars ('\n', '\t', '\r', etc.)

    text.insert(position, str);
    position += str.length();

    if (position < 0)
        position = 0;
    if (position > text.length())
        position = text.length();
    updateText();

    q->markToUpdate();
    q->unlock();
}

void LineEdit::setText(const std::wstring& s)
{
    lock();
    RANGERS_D(LineEdit);
    d->text = s;
    d->position = 0;
    d->stringOffset = 0;
    d->updateText();
    markToUpdate();
    unlock();
}

std::wstring LineEdit::text() const
{
    RANGERS_D(const LineEdit);
    return d->text;
}

int LineEdit::minHeight() const
{
    RANGERS_D(const LineEdit);
    if (d->background && d->label->font())
        return std::max(d->label->font()->size(), (int)d->background->normalHeight());

    if (d->label->font())
        return d->label->font()->size();

    return Widget::minHeight();
}

int LineEdit::minWidth() const
{
    RANGERS_D(const LineEdit);
    std::wstring w = L"W";
    if (d->background && d->label->font())
        return std::max(d->label->font()->calculateStringWidth(w.begin(), w.end()), (int)d->background->normalHeight());

    if (d->label->font())
        return d->label->font()->calculateStringWidth(w.begin(), w.end());

    return Widget::minHeight();
}

int LineEdit::preferredHeight() const
{
    RANGERS_D(const LineEdit);
    if (d->background && d->label->font() && d->style.contentRect.valid())
        return d->background->normalHeight() + d->label->font()->size() - d->style.contentRect.height;

    return minHeight();
}

int LineEdit::maxHeight() const
{
    return preferredHeight();
}
}
