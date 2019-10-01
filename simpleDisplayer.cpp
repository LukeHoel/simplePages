#include "olcPixelGameEngine/olcPixelGameEngine.h"
#include "simpleParser.cpp"
size_t amountPixelsX = 256;
size_t amountPixelsY = 240;

html_element root_element;

class SimpleDisplayer : public olc::PixelGameEngine {

public:
  SimpleDisplayer() { sAppName = "Playground"; }

  bool OnUserCreate() override { return true; }

  // Only does block level elements for now
  void drawElement(html_element& element, int my_offset_x = 0,int my_offset_y = 0){
  			
		FillRect(my_offset_x,my_offset_y,element.style.width,element.style.height,
			olc::Pixel(
			element.style.background_color.red,
			element.style.background_color.green,
			element.style.background_color.blue
		));
		int offset_x = 0;	
		int offset_y = 0;	
		for(html_element child: element.children){
			if(child.style.display != css_display::none){
				drawElement(child, offset_x, offset_y);
				offset_y += child.style.height;
			}
		}
  }
 	
  bool OnUserUpdate(float deltaTime) override {
    Clear(olc::WHITE);
	drawElement(root_element);
    return true;
  }
};

int main(int argc, char **argv) {
  // Parse input
  root_element.name = "Root Element";

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
  if (simpleDisplayer.Construct(amountPixelsX, amountPixelsY, 4, 4))
    simpleDisplayer.Start();

  return 0;
}
