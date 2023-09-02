struct Character {
    unsigned int TextureID; // ID handle of the glyph texture
    float size_x, size_y;      // Size of glyph
    float bearing_x, bearing_y;   // Offset from baseline to left/top of glyph
    unsigned int Advance;   // Horizontal offset to advance to next glyph
};

class TextObject : public Object{
    public:
    TextObject(char * font_path);
    void RenderText(string text, float x, float y, float scale);
    std::vector<Character> Characters;
};