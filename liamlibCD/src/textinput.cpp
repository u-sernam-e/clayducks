#include "textinput.h"
#include <algorithm>
#include <iostream>

// these functions could be improved by using GetKeyPressed() or GetCharPressed() if I ever figure those functions out
std::string getKeysPressed(const std::string& whiteList)
{
    std::string output{};
    for (char c : whiteList)
    {
        if (IsKeyPressed(c))
            output.push_back(c);
    }
    return output;
}
std::string getKeysDown(const std::string& whiteList)
{
    std::string output{};
    for (char c : whiteList)
    {
        if (IsKeyDown(c))
            output.push_back(c);
    }
    return output;
}

// yoinked from raylib example
static void DrawTextBoxedSelectable(Font font, const char *text, Rectangle rec, float fontSize, float spacing, bool wordWrap, Color tint, int selectStart, int selectLength, Color selectTint, Color selectBackTint)
{
    int length = TextLength(text);  // Total length in bytes of the text, scanned by codepoints in loop

    float textOffsetY = 0;          // Offset between lines (on line break '\n')
    float textOffsetX = 0.0f;       // Offset X to next character to draw

    float scaleFactor = fontSize/(float)font.baseSize;     // Character rectangle scaling factor

    // Word/character wrapping mechanism variables
    enum { MEASURE_STATE = 0, DRAW_STATE = 1 };
    int state = wordWrap? MEASURE_STATE : DRAW_STATE;

    int startLine = -1;         // Index where to begin drawing (where a line begins)
    int endLine = -1;           // Index where to stop drawing (where a line ends)
    int lastk = -1;             // Holds last value of the character position

    for (int i = 0, k = 0; i < length; i++, k++)
    {
        // Get next codepoint from byte string and glyph index in font
        int codepointByteCount = 0;
        int codepoint = GetCodepoint(&text[i], &codepointByteCount);
        int index = GetGlyphIndex(font, codepoint);

        // NOTE: Normally we exit the decoding sequence as soon as a bad byte is found (and return 0x3f)
        // but we need to draw all of the bad bytes using the '?' symbol moving one byte
        if (codepoint == 0x3f) codepointByteCount = 1;
        i += (codepointByteCount - 1);

        float glyphWidth = 0;
        if (codepoint != '\n')
        {
            glyphWidth = (font.glyphs[index].advanceX == 0) ? font.recs[index].width*scaleFactor : font.glyphs[index].advanceX*scaleFactor;

            if (i + 1 < length) glyphWidth = glyphWidth + spacing;
        }

        // NOTE: When wordWrap is ON we first measure how much of the text we can draw before going outside of the rec container
        // We store this info in startLine and endLine, then we change states, draw the text between those two variables
        // and change states again and again recursively until the end of the text (or until we get outside of the container).
        // When wordWrap is OFF we don't need the measure state so we go to the drawing state immediately
        // and begin drawing on the next line before we can get outside the container.
        if (state == MEASURE_STATE)
        {
            // TODO: There are multiple types of spaces in UNICODE, maybe it's a good idea to add support for more
            // Ref: http://jkorpela.fi/chars/spaces.html
            if ((codepoint == ' ') || (codepoint == '\t') || (codepoint == '\n')) endLine = i;

            if ((textOffsetX + glyphWidth) > rec.width)
            {
                endLine = (endLine < 1)? i : endLine;
                if (i == endLine) endLine -= codepointByteCount;
                if ((startLine + codepointByteCount) == endLine) endLine = (i - codepointByteCount);

                state = !state;
            }
            else if ((i + 1) == length)
            {
                endLine = i;
                state = !state;
            }
            else if (codepoint == '\n') state = !state;

            if (state == DRAW_STATE)
            {
                textOffsetX = 0;
                i = startLine;
                glyphWidth = 0;

                // Save character position when we switch states
                int tmp = lastk;
                lastk = k - 1;
                k = tmp;
            }
        }
        else
        {
            if (codepoint == '\n')
            {
                if (!wordWrap)
                {
                    textOffsetY += (font.baseSize + font.baseSize/2)*scaleFactor;
                    textOffsetX = 0;
                }
            }
            else
            {
                if (!wordWrap && ((textOffsetX + glyphWidth) > rec.width))
                {
                    textOffsetY += (font.baseSize + font.baseSize/2)*scaleFactor;
                    textOffsetX = 0;
                }

                // When text overflows rectangle height limit, just stop drawing
                if ((textOffsetY + font.baseSize*scaleFactor) > rec.height) break;

                // Draw selection background
                bool isGlyphSelected = false;
                if ((selectStart >= 0) && (k >= selectStart) && (k < (selectStart + selectLength)))
                {
                    DrawRectangleRec(Rectangle{ rec.x + textOffsetX - 1, rec.y + textOffsetY, glyphWidth, (float)font.baseSize*scaleFactor }, selectBackTint);
                    isGlyphSelected = true;
                }

                // Draw current character glyph
                if ((codepoint != ' ') && (codepoint != '\t'))
                {
                    DrawTextCodepoint(font, codepoint, Vector2{ rec.x + textOffsetX, rec.y + textOffsetY }, fontSize, isGlyphSelected? selectTint : tint);
                }
            }

            if (wordWrap && (i == endLine))
            {
                textOffsetY += (font.baseSize + font.baseSize/2)*scaleFactor;
                textOffsetX = 0;
                startLine = endLine;
                endLine = -1;
                glyphWidth = 0;
                selectStart += lastk - k;
                k = lastk;

                state = !state;
            }
        }

        textOffsetX += glyphWidth;
    }
}

void TextInput::update()
{
    if (m_justSubmitted)
        m_txt = "";
    m_justSubmitted = false;
    
    // resizing/repositioning
    if (m_isResizeable)
    {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointCircle(GetMousePosition(), m_pos, m_resizeCrclRadius))
            m_currentlyRepositioning = true;
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointCircle(GetMousePosition(), m_pos + m_size, m_resizeCrclRadius))
            m_currentlyResizing = true;
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
            m_currentlyRepositioning = false;
            m_currentlyResizing = false;
        }

        if (m_currentlyRepositioning) m_pos = GetMousePosition();
        if (m_currentlyResizing) m_size = GetMousePosition() - m_pos;
        if (m_size.x < m_fontSize) m_size.x = m_fontSize;
        if (m_size.y < m_fontSize) m_size.y = m_fontSize;
    }

    // selection/deselection
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        if (CheckCollisionPointRec(GetMousePosition(), {m_pos.x, m_pos.y, m_size.x, m_size.y}))
            m_isSelected = true;
        else
            m_isSelected = false;
    }
    if (IsKeyPressed(KEY_ESCAPE))
        m_isSelected = false;
    

    // selection flashing thing
    m_selectionTimer += lowerLimitFrameTime();
    if (m_selectionTimer > m_selectionFlashRate * 2) m_selectionTimer = 0;

    // typing
    if (m_isSelected)
    {
        static const std::string letterKeys      {"ABCDEFGHIJKLMNOPQRSTUVWXYZ"};
        static const std::string lowerLetterKeys {"abcdefghijklmnopqrstuvwxyz"};

        static const std::string otherTypeableKeys      {"\',-./1234567890;=[\\]` "};
        static const std::string upperOtherTypeableKeys {"\"<_>?!@#$%^&*():+{|}~ "};
        
        if (IsKeyPressed(KEY_CAPS_LOCK)) m_capsLockOn = !m_capsLockOn;
        bool shift{IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)};

        if (m_typeMode == 0) // text mode
        {
            std::string lKeys{getKeysPressed(letterKeys)}; // letters
            for (char& k : lKeys)
            {
                if (m_capsLockOn == shift)
                    k = lowerLetterKeys[letterKeys.find(k)];
            }
            m_txt += lKeys;

            std::string oKeys{getKeysPressed(otherTypeableKeys)}; // other keys
            for (char& k : oKeys)
            {
                if (shift)
                    k = upperOtherTypeableKeys[otherTypeableKeys.find(k)];
            }
            m_txt += oKeys;
        }
        if (m_typeMode == 1) // numbers mode
        {
            if (m_txt.find('.') == -1 && IsKeyPressed(KEY_PERIOD))
                m_txt += '.';
                
            for (int k : getKeysPressed("1234567890"))
                m_txt += static_cast<char>(k);
        }
        if (m_typeMode == 2) // letters mode
        {
            std::string lKeys{getKeysPressed(letterKeys)}; // letters
            for (char& k : lKeys)
            {
                if (m_capsLockOn == shift)
                    k = lowerLetterKeys[letterKeys.find(k)];
            }
            m_txt += lKeys;
        }
        if (m_typeMode == 3) // letters AND numbers (not punctuation & other keys) mode
        {
            for (int k : getKeysPressed("1234567890"))
                m_txt += static_cast<char>(k);

            std::string lKeys{getKeysPressed(letterKeys)}; // letters
            for (char& k : lKeys)
            {
                if (m_capsLockOn == shift)
                    k = lowerLetterKeys[letterKeys.find(k)];
            }
            m_txt += lKeys;
        }

        if (IsKeyPressed(KEY_BACKSPACE) && !m_txt.empty())
        m_txt.pop_back();
        if (IsKeyReleased(KEY_ENTER))
            submit();
    }
}

void TextInput::draw()
{
    if (m_isSelected && m_selectionTimer < m_selectionFlashRate)
        m_txt += '>';

    if (m_isResizeable)
    {
        DrawCircleV(m_pos, m_resizeCrclRadius, CheckCollisionPointCircle(GetMousePosition(), m_pos, m_resizeCrclRadius) || m_currentlyRepositioning ? m_resizeCrclSelectedCol : m_resizeCrclCol);
        DrawCircleV(m_pos + m_size, m_resizeCrclRadius, CheckCollisionPointCircle(GetMousePosition(), m_pos + m_size, m_resizeCrclRadius) || m_currentlyResizing ? m_resizeCrclSelectedCol : m_resizeCrclCol);
    }
    DrawRectangleV(m_pos, m_size, m_bkgrndCol);

    Font f{m_font.empty() ? GetFontDefault() : fontStrg().get(m_font, 0)};

    DrawTextBoxedSelectable(f, m_txt.c_str(), {m_pos.x, m_pos.y, m_size.x, m_size.y}, m_fontSize, m_fontSpacing, true, m_txtCol, 0, 0, WHITE, BLUE);

    if (m_isSelected && m_selectionTimer < m_selectionFlashRate)
        m_txt.pop_back();
}