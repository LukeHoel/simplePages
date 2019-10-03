#include "../olcPixelGameEngine/olcPixelGameEngine.h"
#include "css/css.h"
#include "html/html.h"
#include "utils/utils.h"
// 1080p
//size_t amountPixelsX = 1920;
//size_t amountPixelsY = 1080;
size_t amountPixelsX = 1280;
size_t amountPixelsY = 720;

html_element root_element;

class SimpleDisplayer : public olc::PixelGameEngine {

public:
  SimpleDisplayer() { sAppName = "Playground"; }

  // Only does block level elements for now
  void drawElement(html_element &element, int my_offset_x = 0,
                   int my_offset_y = 0) {
    if (element.style.background_color.is_set()) {
      FillRect(my_offset_x, my_offset_y, element.calculated_width,
               element.calculated_height,
               olc::Pixel(element.style.background_color.red,
                          element.style.background_color.green,
                          element.style.background_color.blue));
    }
    int offset_x = 0;
    int offset_y = 0;
    for (html_element child : element.children) {
      if (child.style.display != css_display::none) {
	  	if(child.isTextOnly){
			for(int i = 0; i < child.text.size(); i++){
			DrawString(my_offset_x + offset_x, my_offset_y + offset_y, child.text.substr(i,1),
				olc::Pixel(element.style.color.red,
							element.style.color.green,
							element.style.color.blue
							), 1);
				offset_x += 8;
				if(isspace(child.text[i]) && offset_x > element.calculated_width){
					offset_x = 0;
        			offset_y += 8;
				}
			}
		}
		else{
        	drawElement(child, my_offset_x + offset_x, my_offset_y + offset_y);
        	offset_y += child.calculated_height;
		}
      }
    }
  }

  void redraw() {
    Clear(olc::WHITE);
    drawElement(root_element);
  }
  bool OnUserCreate() override {
    redraw();
    return true;
  }

  bool OnUserUpdate(float deltaTime) override { return true; }
};

int main(int argc, char **argv) {
  // Parse input
  root_element.name = "Root Element";
  root_element.calculated_width = amountPixelsX;
  root_element.calculated_height= amountPixelsY;
  if (argc > 1) {
    std::ifstream file(argv[1]);
    file.unsetf(std::ios_base::skipws);
    read_into_element(root_element, file);
  } else {
    std::cin.unsetf(std::ios_base::skipws);
    read_into_element(root_element, std::cin);
  }
  print_element(root_element);
  SimpleDisplayer simpleDisplayer;
  if (simpleDisplayer.Construct(amountPixelsX, amountPixelsY, 1, 1))
    simpleDisplayer.Start();

  return 0;
}

#include "css/css.cpp"
#include "html/html.cpp"
#include "utils/utils.cpp"
