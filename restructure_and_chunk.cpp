#include <fstream>
#include <iostream>
#include "include/json.hpp" // Include this header for the JSON library

int main()
{
    std::ifstream inFile("big.json");
    if (!inFile.is_open())
    {
        std::cerr << "Failed to open file." << std::endl;
        return 1;
    }

    char ch;
    int depth = 0;
    bool inString = false;
    bool escape = false;
    std::string currentObject;
    int fileCount = 1;

    while (inFile.get(ch))
    {
        currentObject += ch;
        if (ch == '\"' && !escape)
        {
            inString = !inString;
        }
        escape = (ch == '\\' && !escape);

        if (!inString)
        {
            if (ch == '{')
            {
                depth++;
            }
            else if (ch == '}')
            {
                depth--;
                if (depth == 0)
                {
                    try
                    {
                        auto json = nlohmann::json::parse(currentObject);
                        nlohmann::json hotels = json["Item"]["hotels"]["M"];
                        std::string base_id = json["Item"]["id"]["N"].get<std::string>();
                        int max_hotels = 1000;
                        std::vector<std::string> hotel_keys(hotels.size());
                        int k = 0;
                        for (auto &item : hotels.items())
                        {
                            hotel_keys[k++] = item.key();
                        }
                        int num_chunks = (hotel_keys.size() + max_hotels - 1) / max_hotels;

                        std::string concatenatedJson; // String to concatenate all chunks
                        int nextHotelIdStart = 0;     // To track hotel IDs across chunks

                        for (int i = 0; i < num_chunks; i++)
                        {
                            nlohmann::json new_hotels;
                            int start_index = i * max_hotels;
                            int end_index = std::min((i + 1) * max_hotels, (int)hotel_keys.size());

                            for (int j = start_index; j < end_index; j++)
                            {
                                int new_key = nextHotelIdStart++;
                                new_hotels[std::to_string(new_key)] = hotels[hotel_keys[j]];
                            }

                            nlohmann::json new_item = json["Item"];
                            new_item["hotels"]["M"] = new_hotels;
                            new_item["id"]["N"] = base_id + "." + std::to_string(i + 1);
                            concatenatedJson += new_item.dump() + (i < num_chunks - 1 ? "" : "");
                        }
                        currentObject.clear();

                        std::ofstream outFile("big" + std::to_string(fileCount++) + ".json");
                        if (!outFile.is_open())
                        {
                            std::cerr << "Failed to open output file." << std::endl;
                            return 1;
                        }
                        outFile << concatenatedJson;
                        outFile.close();
                    }
                    catch (const nlohmann::json::exception &e)
                    {
                        std::cerr << "JSON parse error: " << e.what() << '\n';
                        return 1;
                    }
                }
            }
        }
    }

    inFile.close();
    return 0;
}
