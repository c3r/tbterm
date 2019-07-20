#include <utility>
#include <algorithm>
#include <cstdint>
#include <cstring>
#include "textbuffer.h"

void TextBuffer::Resize(uint32_t w, uint32_t h) {
 	std::vector<uint32_t> characters;
	std::vector<CharAttrib> attributes;   

    const size_t sz = (size_t)w * (size_t)h;
    characters.resize(sz);
    attributes.resize(sz);

    uint32_t min_w = std::min(w, _w);
    for (uint32_t j = 0; j < std::min(h, _h); j++) {
        memcpy(
            &characters[j * w],
            &_characters[j * _w],
            sizeof(decltype(characters)::value_type) * min_w);

        memcpy(
            &attributes[j * w],
            &_attributes[j * _w],
            sizeof(decltype(attributes)::value_type) * min_w);
    }

    _characters = std::move(characters);
    _attributes = std::move(attributes);

    _h = h;
    _w = w;
}

TextBuffer::TextBuffer() {
	Reset();	   
}

TextBuffer::TextBuffer(uint32_t w, uint32_t h) {
	Reset();
	Resize(w, h);
}

void TextBuffer::InsertCharacter(uint32_t character) {
	InsertCharacter(character, _current_attribute);	
}

void TextBuffer::InsertCharacter(uint32_t character, CharAttrib attribute) {
	
}

void TextBuffer::ChangeAttribute(CharAttrib attribute) {
	// TODO: tutaj caly obiekt sie kopiuje!
	_current_attribute = attribute;
}

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

void TextBuffer::ChangeAuxiliaryAttribute(uint8_t aux) {
	_current_attribute.aux = aux;
}

void TextBuffer::Clear() {
	std::fill(_characters.begin(), _characters.end(), 0);
}

void TextBuffer::Reset() {
	_w = 0;
	_h = 0;
	cursor_x = 0;
	cursor_y = 0; 
	scroll_y = 0;
	_current_attribute = kDefaultAttribute;
	Clear();
}
