#pragma once
#include <cstdint>
#include <utility>
#include <vector>

struct Color {
    uint8_t r, g, b;
};

struct CharAttrib {
	Color fg, bg;
	uint8_t aux; // 0 - bold
};

class TextBuffer {

	private:
	// TODO: if scrolling the buffer after it's full is slow
	// implement a line-based ringbuffer.

	std::vector<uint32_t> _characters;
	std::vector<CharAttrib> _attributes;
	uint32_t _w, _h; // In chars.

	const CharAttrib kDefaultAttribute = {
		{ 192, 192, 192 },
		{ 0, 0, 0 },
		0
	};

	CharAttrib _current_attribute = kDefaultAttribute;

	uint32_t _cursor_x;
	uint32_t _cursor_y; // In scrollback buffer space.
	uint32_t _scroll_y;

	void WrapCursorIfNeeded(); // And scroll if needed

	public:
	TextBuffer();
	TextBuffer(uint32_t w, uint32_t h);
	std::vector<uint32_t>&  GetCharacters() const;
	std::vector<CharAttrib>&  GetAttributes() const;

	void Resize(uint32_t w, uint32_t h); // In characters.

	void InsertCharacter(uint32_t character);
	void InsertCharacter(uint32_t character, CharAttrib attribute);
	void LineFeed(); // Move cursor to new line.
	void CarriageReturn();

	void ChangeAttribute(CharAttrib attribute);

	void ChangeForegroundColor(uint8_t r, uint8_t g, uint8_t b);
	void ChangeBackgroundColor(uint8_t r, uint8_t g, uint8_t b);
	void ChangeAuxiliaryAttribute(uint8_t aux);
	void Clear();
	void Reset();

	std::pair<uint32_t, uint32_t> GetCursorPosition() const;
};
