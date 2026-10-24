#pragma once
//
// REAPER RGB -> UF8 palette index.
//
// The UF8 quantizes to a small palette (16 entries, 0x00..0x0F hypothesis).
// We replicate that mapping here. For entries we've observed in captures we
// use their exact RGB. For the 5 unmapped entries (0x00, 0x01, 0x05, 0x0D,
// 0x0F) we include TBD stubs — nearest-match still works, but until we
// measure them they may be attributed incorrectly.
//

#include <array>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace uf8 {

struct Rgb {
    uint8_t r, g, b;
};

// Map REAPER track color (24-bit RGB as 0xRRGGBB, low 24 bits) to a UF8
// palette index in 0..15. Uses nearest-match in RGB space against the
// reference palette below.
uint8_t quantize(Rgb color);

// Exposed for tests and for the case where a caller already has an 0xRRGGBB
// value from REAPER's GetTrackColor (which returns 0x01000000 | 0x00RRGGBB).
uint8_t quantize(uint32_t rgb24);

// Reference palette: what each confirmed index looks like. Entries that are
// still unmapped (0x00, 0x01, 0x05, 0x0D, 0x0F) return std::nullopt.
std::optional<Rgb> paletteEntry(uint8_t index);

// The word for an index, for anywhere a person has to pick one. Never nullptr;
// an index with no colour answers "off". SSL 360's own names (LedColourType),
// in one place so nobody writes a second list.
const char* paletteName(uint8_t index);

// The colour SSL 360 draws for that name, for a swatch beside it. Not the
// quantiser's reference (paletteEntry): that one is what a UF8 looked like.
std::optional<Rgb> paletteSwatch(uint8_t index);

// ⇨ COLOUR → LAMP TABLE. No distance metric reproduces what SSL 360 lights for
// a given REAPER colour, so a session palette built against SSL 360 can land on
// other lamps here. An entry pins one exact 24-bit colour (0xRRGGBB) to one
// palette index; quantize() answers it before any matching, greys included,
// and every colour not in the table quantizes as before. Settings → Appearance
// edits it; it persists as ExtState rea_sixty / uf8_colour_map.
struct ColourOverride {
    uint32_t rgb;     // 0xRRGGBB
    uint8_t  index;   // 0x00..0x0C
};
void setColourOverrides(std::vector<ColourOverride> list);
std::vector<ColourOverride> colourOverrides();
// "RRGGBB=II;RRGGBB=II;…" (hex). Parsing skips anything malformed.
std::string formatColourOverrides(const std::vector<ColourOverride>& list);
std::vector<ColourOverride> parseColourOverrides(std::string_view text);

} // namespace uf8
