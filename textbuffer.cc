#include <utility>
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <cstdio>
#include "textbuffer.h"

void TextBuffer::Resize(uint32_t w, uint32_t h) 
{
    std::vector<uint32_t> characters;
    std::vector<CharAttrib> attributes;
    const size_t sz = (size_t)w * (size_t)h;
    characters.resize(sz);
    attributes.resize(sz);
    uint32_t min_w = std::min(w, _w);

    for (uint32_t j = 0; j < std::min(h, _h); j++) 
    {
	memcpy( &characters[j * w],
		&_characters[j * _w],
		sizeof(decltype(characters)::value_type) * min_w);

	memcpy( &attributes[j * w],
		&_attributes[j * _w],
		sizeof(decltype(attributes)::value_type) * min_w);
    }

    _characters = std::move(characters); 
    _attributes = std::move(attributes);
    _h = h;
    _w = w;
    _cursor_x = std::min(_cursor_x, _w);
}

TextBuffer::TextBuffer() { Reset(); }
TextBuffer::TextBuffer(uint32_t w, uint32_t h) { Reset(); Resize(w, h); }
void TextBuffer::InsertCharacter(uint32_t character) { InsertCharacter(character, _current_attribute); }

void TextBuffer::LineFeed() {
    _cursor_y++;
    if (_cursor_y < _h) { return; }

    // Scroll character and attribute buffers one line up.
    memmove( &_characters[0],
	    &_characters[1 * _w],
	    sizeof(uint32_t) * _w * (_h-1));

    memmove( &_attributes[0],
	    &_attributes[1 * _w],
	    sizeof(CharAttrib) * _w * (_h-1));

    // Clean last line (oinly chars, leave attribues as is).
    memset( &_characters[(_h - 1) * _w], 0, sizeof(uint32_t) * _w);
    if (_cursor_x > 0) { _cursor_y = _h - 1; }
}

void TextBuffer::CarriageReturn() { _cursor_x = 0; }
void TextBuffer::WrapCursorIfNeeded() {
    // No need to scroll or move cursor to new line.
    if (_cursor_x < _w && _cursor_y < _h) { return; }
    if (_cursor_x > _w) { _cursor_y++; _cursor_x = 0; }
    if (_cursor_y >= _h) { LineFeed(); CarriageReturn(); }
}

void TextBuffer::InsertCharacter(uint32_t character, CharAttrib attribute) {
    WrapCursorIfNeeded();
    size_t idx = _cursor_x + _cursor_y * _w;
    _characters.insert(_characters.begin() + idx, character);
    _attributes.insert(_attributes.begin() + idx, attribute);
    _cursor_x++;
    WrapCursorIfNeeded();
}

// TODO: tutaj caly obiekt sie kopiuje!
void TextBuffer::ChangeAttribute(CharAttrib attribute) { _current_attribute = attribute; }

void TextBuffer::ChangeForegroundColor(uint8_t r, uint8_t g, uint8_t b) {
    _current_attribute.fg.r = r;
    _current_attribute.fg.g = g;
    _current_attribute.fg.b = b;
}

void TextBuffer::ChangeBackgroundColor(uint8_t r, uint8_t g, uint8_t b) {
    _current_attribute.bg.r = r;
    _current_attribute.bg.g = g;
    _current_attribute.bg.b = b;
}

void TextBuffer::ChangeAuxiliaryAttribute(uint8_t aux) { _current_attribute.aux = aux; }
void TextBuffer::Clear() { std::fill(_characters.begin(), _characters.end(), 0); }

void TextBuffer::Reset() {
    _w = 0;
    _h = 0;
    _cursor_x = 0;
    _cursor_y = 0;
    _scroll_y = 0;
    _current_attribute = kDefaultAttribute; 
    Clear();
}

std::pair<uint32_t, uint32_t> TextBuffer::GetCursorPosition() const { return std::make_pair(_cursor_x, _cursor_y); }
