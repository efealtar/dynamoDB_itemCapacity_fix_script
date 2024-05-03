#include <iostream>
#include <fstream>
#include <sstream>

int main()
{
    std::ifstream input("output.json", std::ifstream::binary);
    if (!input.is_open())
    {
        std::cerr << "Failed to open file\n";
        return 1;
    }

    std::ofstream big("big.json", std::ofstream::binary | std::ofstream::trunc);
    std::ofstream modifiedOutput("modified_output.json", std::ofstream::binary | std::ofstream::trunc);

    std::ostringstream objectStream;
    char ch;
    int braceCount = 0;
    bool inObject = false;

    while (input.get(ch))
    {
        if (ch == '{')
        {
            braceCount++;
            inObject = true;
        }

        if (inObject)
        {
            objectStream << ch;
        }

        if (ch == '}')
        {
            braceCount--;
            if (braceCount == 0)
            {
                inObject = false;
                std::string jsonObject = objectStream.str();
                if (jsonObject.size() >= 400000)
                { // greater than or equal to 400 KB
                    big << jsonObject << "\n";
                }
                else
                {
                    modifiedOutput << jsonObject << "\n";
                }
                objectStream.str(""); // Clear the stream for the next object
                objectStream.clear();
            }
        }
    }

    input.close();
    big.close();
    modifiedOutput.close();

    return 0;
}
