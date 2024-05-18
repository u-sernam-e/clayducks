#ifndef TEXTINPUT
#define TEXTINPUT
#include "ob.h"
#include "rayextended.h"
#include "fontstorage.h"
#include <string>

class TextInput : public Ob
{
private:
    // technically any of these could be public vars
    Vector2 m_pos{100, 100};
    Vector2 m_size{500, 300};
    bool m_isResizeable{true};
    int m_typeMode{0}; // 0 for all text, 1 for just numbers (including decimal point), 2 for just letters, 3 for letters and numbers (no punctuation or decimal point)
    std::string m_font{""};
    int m_fontSize{20};
    int m_fontSpacing{2};
    Color m_bkgrndCol{GRAY};
    Color m_txtCol{BLACK};
    Color m_selectedTxtCol{MAROON};
    Color m_resizeCrclCol{DARKGRAY};
    Color m_resizeCrclSelectedCol{LIGHTGRAY};
    int m_resizeCrclRadius{10};
    float m_selectionFlashRate{.25};
    bool m_hidden{false};
    
    std::string m_txt{};
    bool m_isSelected{};
    float m_selectionTimer{};
    bool m_justSubmitted{};
    bool m_currentlyRepositioning{};
    bool m_currentlyResizing{};
    int m_capsLockOn{};

public:
    TextInput(Vector2 pos={100, 100},
            Vector2 size={500, 300},
            bool isResizeable=true,
            int typeMode=0,
            std::string font="",
            int fontSize=20,
            int fontSpacing=2,
            Color bkgrndCol=GRAY,
            Color txtCol=BLACK,
            Color selectedTxtCol=MAROON,
            Color resizeCrclCol=DARKGRAY,
            Color resizeCrclSelectedCol=LIGHTGRAY,
            int resizeCrclRadius=10,
            float selectionFlashRate=.25)
        : m_pos{pos}
        , m_size{size}
        , m_isResizeable{isResizeable}
        , m_typeMode{typeMode}
        , m_font{font}
        , m_fontSize{fontSize}
        , m_fontSpacing{fontSpacing}
        , m_bkgrndCol{bkgrndCol}
        , m_txtCol{txtCol}
        , m_selectedTxtCol{selectedTxtCol}
        , m_resizeCrclCol{resizeCrclCol}
        , m_resizeCrclSelectedCol{resizeCrclSelectedCol}
        , m_resizeCrclRadius{resizeCrclRadius}
        , m_selectionFlashRate{selectionFlashRate}
    {}

    void update() override;
    void draw() override;

    // output
    const std::string& getInputTxt() { return m_txt; }
    int getInputInt() { return atoi(m_txt.c_str()); }
    float getInputFloat() { return atof(m_txt.c_str()); }
    bool justSubmitted() { return m_justSubmitted; }
    // input
    void submit() { m_justSubmitted = true; }
    void setSelect(bool selection) { m_isSelected = selection; }
    void setResizeable(bool isResizeable) { m_isResizeable = isResizeable; }
    void setTypeMode(int mode) { submit(); m_typeMode = mode; }

    void setPos(Vector2 p) {m_pos = p;}
    Vector2 getPos() {return m_pos;}

    void setSize(Vector2 s) {m_size = s;}
    Vector2 getSize() {return m_size;}
};  

#endif