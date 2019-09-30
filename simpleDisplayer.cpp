#include "olcPixelGameEngine/olcPixelGameEngine.h"
#include "simpleParser.cpp"
size_t amountPixelsX = 256;
size_t amountPixelsY = 240;

class SimpleDisplayer : public olc::PixelGameEngine {

public:
  SimpleDisplayer() { sAppName = "Playground"; }

  bool OnUserCreate() override { return true; }

  bool OnUserUpdate(float deltaTime) override {
    Clear(olc::BLACK);

    return true;
  }
};

int main(int argc, char **argv) {
  // Parse input
  html_element root_element;
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
