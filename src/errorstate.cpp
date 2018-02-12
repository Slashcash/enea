#include "errorstate.hpp"

ErrorState::ErrorState(sf::RenderWindow* theWindow, const Result& theError) : State(theWindow) {
    init_error = theError;

    error_text.setFont(font_manager.getFont("share/fonts/dtmsans.otf"));
    error_text.setString("Initialization failed with error "+init_error.getDescription()+" "+std::to_string(init_error.getErrorNumber()));
    scene.attachToLayer(&error_text, 0);
}

void ErrorState::updateEffectively() {

}

void ErrorState::onNotify(const Signal& theSignal) {
    if( theSignal.getName() == "INPUT_EXIT" ) exit();
}
