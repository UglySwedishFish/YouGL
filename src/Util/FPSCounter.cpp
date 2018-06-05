#include "FPSCounter.h"

#include "../ResourceManager/ResourceHolder.h"
#include "../Renderer/Renderer.h"

#include <sstream>
#include <iomanip>

FPSCounter::FPSCounter()
{
    m_text.move(0, 0);
    m_text.setOutlineColor (sf::Color::Black);
    m_text.setFillColor({255,255,255});
    m_text.setOutlineThickness  (2);
    m_text.setFont(ResourceHolder::get().fonts.get("arial"));
    m_text.setCharacterSize(15);
}

//updates the FPS variable
void FPSCounter::update()
{
    m_frameCount++;

    if (m_delayTimer.getElapsedTime().asSeconds() > 0.2) {
        float fps = m_frameCount / m_fpsTimer.restart().asSeconds();
        m_frameCount = 0;
        m_delayTimer.restart();
        std::ostringstream stream;
        stream << "Frame time: ";
        stream << std::setprecision(2) << std::fixed << std::showpoint << (1000.0f / fps) << '\n';
        m_text.setString(stream.str());
        
    }
}

//Draws the FPS display to the window
void FPSCounter::draw(Renderer& renderer)
{
    renderer.draw(m_text);
}
