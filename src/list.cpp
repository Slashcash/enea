#include "list.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <iostream>

List::List() {
    page = 0;
    spacing = 20;
    truncate_height = 0;
    truncate_width = 0;
    main_color = sf::Color::White;
    selected = 0;
    style = Normal;
}

List::List(const std::vector<Row>& theRows) : List() {
    rows = theRows;
    calculatePages();
}

void List::drawCurrent(sf::RenderTarget& theRenderTarget, sf::RenderStates theRenderStates) const {
    //empty because we attach draw to this and they get drawn by themselves

}

void List::setRowVector(const std::vector<Row>& theRows) {
    rows.clear();
    for(auto it = theRows.begin(); it < theRows.end(); it++) rows.push_back(*it);
    for(auto it = rows.begin(); it < rows.end(); it++) it->setActive(false);
    setAllParameters();
    calculatePages();
}

void List::calculatePages() {
    pages.clear(); //resetting all the pages

    if(!rows.empty()) {
        if( truncate_height < 0 ) truncate_height = 0; //just in case someone wants to be funny and setting a negative value

        if( truncate_height == 0 ) { std::vector<Row*> buffer; for(auto it = rows.begin(); it < rows.end(); it++) buffer.push_back(&(*it)); pages.push_back(buffer); }

        else { //algorithm to calculate how many element to add in a page
            std::vector<Row*> buffer;
            float page_height = 0;
            for(unsigned int i = 0; i < rows.size(); i++) {
                buffer.push_back(&rows[i]);
                page_height = page_height + rows[i].getLocalBounds().height + spacing;
                if( page_height >= truncate_height || i == rows.size() - 1 ) { pages.push_back(buffer); page_height = 0; buffer.clear(); }
            }
        }

        //set the positions
        if( style == Normal )
            for(unsigned i = 0; i < pages.size(); i++ )
                for(unsigned int j = 0; j < pages[i].size(); j++)
                    if(j == 0) pages[i][j]->setPosition(0, 0);
                    else { pages[i][j]->setPosition(0, pages[i][j-1]->getLocalBounds().height + spacing); pages[i][j-1]->attachChild(pages[i][j]); }

        else if( style == Centered ) {
            for(unsigned i = 0; i < pages.size(); i++ ) {
                for(unsigned int j = 0; j < pages[i].size(); j++) {
                    pages[i][j]->setOrigin(pages[i][j]->getLocalBounds().width/2, 0);
                    if(j == 0) pages[i][j]->setPosition(0, 0);
                    else { pages[i][j]->setPosition(pages[i][j-1]->getLocalBounds().width/2, pages[i][j-1]->getLocalBounds().height + spacing); pages[i][j-1]->attachChild(pages[i][j]); }
                }
            }
        }
    }

    //set the selection back to zero for safety reasons
    setSelected(0);
    setPage(0);
}

void List::setAllParameters() {
    setTruncateWidth(truncate_width);
    setMainColor(main_color);
    setHighLightColor(highlight_color);
    setUpdateRate(getUpdateRate());
    setActive(isActive());

}

float List::getWidth() const {
    std::vector<float> width;

    if( rows.empty() ) return 0;

    else {
        for( auto it = pages[page].begin(); it < pages[page].end(); it++ ) width.push_back((*it)->getLocalBounds().width);
        return *std::max_element(width.begin(), width.end());
    }
}

float List::getHeight() const {
    float total_height = 0;

    if( !rows.empty() ) {
        for( auto it = pages[page].begin(); it < pages[page].end(); it++) total_height = total_height + (*it)->getLocalBounds().height + spacing;
        return total_height - spacing;
    }

    else return 0;
}

void List::setSelected(const unsigned int theSelection) {
    if( !rows.empty() && isActive() && theSelection >= 0 && theSelection < rows.size() ) {
        rows[selected].setColor(main_color);
        rows[selected].setActive(false);
        selected = theSelection;
        rows[selected].setColor(highlight_color);
        rows[selected].setActive(true);
        unsigned int next_page = 0;
        unsigned int element_in_page = pages[next_page].size();
        while( selected >= element_in_page ) { element_in_page = element_in_page + pages[next_page].size(); next_page++; }
        if( next_page != page ) setPage(next_page);
    }
}

void List::setActive(const bool isActive) {
    Node::setActive(isActive);
    if(!rows.empty()) {
        if(isActive) {
            rows[selected].setActive(true);
            rows[selected].setColor(highlight_color);
        }

        else {
            rows[selected].setActive(false);
            rows[selected].setColor(main_color);
        }
    }
}

 void List::setPage(const unsigned int thePage) {
    if( thePage >= 0 && (thePage < pages.size() || pages.empty()) ) {
        page = thePage;
        detachAllChildren();
        if( !pages.empty() ) attachChild(pages[page][0]);
    }
}

