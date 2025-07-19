#include <windows.h>
#include <commdlg.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <SFML/Graphics.hpp>
#include <iomanip>
#include <vector>
#include <cmath>

using namespace std;

// ----------- FileLoader --------------
class FileLoader
{
private:
    wstring filePathW;
    string filePathA;

public:
    bool selectFile()
    {
        wchar_t pathBuffer[MAX_PATH] = L"";

        OPENFILENAMEW ofn = {0};
        ofn.lStructSize = sizeof(ofn);
        ofn.lpstrFilter = L"Text Files\0*.txt\0All Files\0*.*\0";
        ofn.lpstrFile = pathBuffer;
        ofn.nMaxFile = MAX_PATH;
        ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
        ofn.lpstrTitle = L"Select a DNA Report File";

        if (GetOpenFileNameW(&ofn))
        {
            filePathW = pathBuffer;
            filePathA = ws2s(filePathW);
            return true;
        }
        return false;
    }

    string getFilePath() const
    {
        return filePathA;
    }

private:
    string ws2s(const wstring &wstr)
    {
        int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
        string strTo(size_needed, 0);
        WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
        return strTo;
    }
};

// ----------- DNAParser ---------------------
class DNAParser
{
private:
    unordered_set<string> targetSNPs = {
        "rs12913832", "rs1800407", "rs12896399",
        "rs1393350", "rs16891982", "rs12203592"};

public:
    unordered_map<string, string> parseFile(const string &filePath)
    {
        unordered_map<string, string> snpMap;
        ifstream file(filePath);
        if (!file.is_open())
        {
            cerr << "Error opening file: " << filePath << endl;
            return snpMap;
        }

        string line;
        while (getline(file, line))
        {
            if (line.empty() || line[0] == '#')
                continue;
            istringstream iss(line);
            string rsid, chromosome, position, genotype;
            if (!(iss >> rsid >> chromosome >> position >> genotype))
                continue;

            if (targetSNPs.count(rsid))
            {
                snpMap[rsid] = genotype;
            }
        }
        return snpMap;
    }

    void printSNPs(const unordered_map<string, string> &snps, const string &label)
    {
        cout << "\n== " << label << " Genotypes ==" << endl;
        for (const auto &snp : targetSNPs)
        {
            auto it = snps.find(snp);
            if (it != snps.end())
                cout << snp << ": " << it->second << endl;
            else
                cout << snp << ": Not Found" << endl;
        }
    }
};

// ----------- Score Class ------------------
class Score
{
private:
    double blue, green, brown;

public:
    Score() : blue(0), green(0), brown(0) {}
    Score(double b, double g, double br) : blue(b), green(g), brown(br) {}

    double getBlue() const { return blue; }
    double getGreen() const { return green; }
    double getBrown() const { return brown; }
};

// ----------- SNPData Class ------------------
class SNPData
{
private:
    double weight;
    unordered_map<string, Score> genotypeScores;

public:
    SNPData() : weight(0.0) {}

    void setWeight(double w) { weight = w; }
    double getWeight() const { return weight; }

    void addGenotypeScore(const string &genotype, const Score &score)
    {
        genotypeScores[genotype] = score;
    }

    bool hasGenotype(const string &genotype) const
    {
        return genotypeScores.find(genotype) != genotypeScores.end();
    }

    Score getScore(const string &genotype) const
    {
        auto it = genotypeScores.find(genotype);
        if (it != genotypeScores.end())
            return it->second;
        return Score();
    }
};
class EyeVisualization
{
private:
    sf::Sprite eyeSprite;
    sf::Texture eyeTexture;
    sf::Text eyeLabel;
    sf::Text eyeHeading;
    string colorName;
    sf::Color tintColor;

public:
    EyeVisualization(const sf::Font &font, double bluePercent, double greenPercent, double brownPercent)
    {
        if (!eyeTexture.loadFromFile("eye.png"))
        {
            cerr << "Error: Failed to load eye.png - make sure it's in the same directory\n";
            exit(1);
        }

        eyeTexture.setSmooth(true);
        eyeSprite.setTexture(eyeTexture, true);

        sf::Vector2u texSize = eyeTexture.getSize();
        eyeSprite.setOrigin(texSize.x / 2.f, texSize.y / 2.f);
        float scale = 300.f / std::max(texSize.x, texSize.y);
        eyeSprite.setScale(scale, scale);

        if (brownPercent >= bluePercent && brownPercent >= greenPercent)
        {
            colorName = "Brown";
            tintColor = sf::Color(101, 67, 33);
        }
        else if (bluePercent >= greenPercent)
        {
            colorName = "Blue";
            tintColor = sf::Color(0, 105, 148);
        }
        else
        {
            colorName = "Green";
            tintColor = sf::Color(34, 139, 34);
        }

        eyeSprite.setColor(tintColor);

        eyeLabel.setFont(font);
        eyeLabel.setCharacterSize(24);
        eyeLabel.setFillColor(sf::Color::Black);
        eyeLabel.setString("Predicted: " + colorName + " Eyes");

        eyeHeading.setFont(font);
        eyeHeading.setCharacterSize(28);
        eyeHeading.setFillColor(sf::Color::Black);
        eyeHeading.setString("Visual Prediction");
    }

    void setPosition(float x, float y)
    {
        eyeSprite.setPosition(x, y);

        sf::FloatRect labelBounds = eyeLabel.getLocalBounds();
        eyeLabel.setOrigin(labelBounds.width / 2, 0);
        eyeLabel.setPosition(x, y + 180);

        sf::FloatRect headBounds = eyeHeading.getLocalBounds();
        eyeHeading.setOrigin(headBounds.width / 2, 0);
        eyeHeading.setPosition(x, y - 200);
    }

    void draw(sf::RenderWindow &window)
    {
        window.draw(eyeHeading);
        window.draw(eyeSprite);
        window.draw(eyeLabel);
    }
};

// ----------- EyeColorOutput Class  ------------------
class EyeColorOutput
{
private:
    double blue, green, brown;

public:
    EyeColorOutput(double b, double g, double br) : blue(b), green(g), brown(br) {}

    void showOutput()
    {
        sf::RenderWindow window(sf::VideoMode(1200, 800), "Eye Color Prediction Report");

        sf::Font font;
        if (!font.loadFromFile("arial.ttf"))
        {
            if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf"))
            {
                if (!font.loadFromFile("/usr/share/fonts/truetype/arial.ttf"))
                {
                    cerr << "Error loading font. Using default.\n";
                }
            }
        }

        EyeVisualization eyeVis(font, blue, green, brown);
        eyeVis.setPosition(850, 300);

        sf::Text title;
        title.setFont(font);
        title.setCharacterSize(36);
        title.setFillColor(sf::Color::Black);
        title.setStyle(sf::Text::Bold); 
        title.setString("Child's Eye Color Prediction");

        sf::FloatRect titleBounds = title.getLocalBounds();
        title.setOrigin(titleBounds.width / 2, 0);
        title.setPosition(600, 30); 

        string mostLikely = getMostLikelyResult();
        sf::Text resultText;
        resultText.setFont(font);
        resultText.setCharacterSize(28);
        resultText.setFillColor(sf::Color::Black);
        resultText.setString("Most Likely: " + mostLikely);
        resultText.setPosition(50, 100);

        sf::RectangleShape tableBorder(sf::Vector2f(400, 180));
        tableBorder.setFillColor(sf::Color::Transparent);
        tableBorder.setOutlineColor(sf::Color::Black);
        tableBorder.setOutlineThickness(2);
        tableBorder.setPosition(50, 180);

        vector<string> colors = {"Blue", "Green", "Brown"};
        vector<double> percentages = {blue, green, brown};
        vector<sf::Text> tableTexts;

        for (int i = 0; i < 3; ++i)
        {
            sf::Text row;
            row.setFont(font);
            row.setCharacterSize(24);
            row.setFillColor(sf::Color::Black);

            ostringstream oss;
            oss << fixed << setprecision(1) << percentages[i];

            row.setString(colors[i] + " Eyes:\t\t" + oss.str() + " %");
            row.setPosition(60, 200 + i * 50);
            tableTexts.push_back(row);
        }

       
        auto pieChart = createPieChart(percentages,
                                       {sf::Color::Blue, sf::Color::Green, sf::Color(139, 69, 19)},
                                       300, 555, 150); 

        vector<sf::Text> pieLabels;
        vector<sf::RectangleShape> colorSwatches;

       
        for (int i = 0; i < 3; ++i)
        {
            sf::RectangleShape swatch(sf::Vector2f(20, 20));
            swatch.setFillColor(i == 0 ? sf::Color::Blue : (i == 1 ? sf::Color::Green : sf::Color(139, 69, 19)));
            swatch.setPosition(50, 670 + i * 30); // moved down from 450

            colorSwatches.push_back(swatch);

            sf::Text label;
            label.setFont(font);
            label.setCharacterSize(20);
            label.setFillColor(sf::Color::Black);

            ostringstream oss;
            oss << fixed << setprecision(1) << percentages[i];

            label.setString(colors[i] + ": " + oss.str() + " %");
            label.setPosition(80, 670 + i * 30); 
            pieLabels.push_back(label);
        }

      
        while (window.isOpen())
        {
            sf::Event event;
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                    window.close();
            }

            window.clear(sf::Color::White);

            window.draw(title);
            window.draw(resultText);
            window.draw(tableBorder);
            for (auto &t : tableTexts)
                window.draw(t);
            for (auto &s : pieChart)
                window.draw(s);
            for (auto &l : pieLabels)
                window.draw(l);
            for (auto &s : colorSwatches)
                window.draw(s);

            eyeVis.draw(window);

            window.display();
        }
    }

private:
    string getMostLikelyResult()
    {
        double maxVal = max({blue, green, brown});
        if (maxVal == blue)
            return "Blue Eyes (" + to_string((int)round(blue)) + "%)";
        else if (maxVal == green)
            return "Green Eyes (" + to_string((int)round(green)) + "%)";
        else
            return "Brown Eyes (" + to_string((int)round(brown)) + "%)";
    }

    vector<sf::ConvexShape> createPieChart(const vector<double> &values, const vector<sf::Color> &colors, int cx, int cy, int radius)
    {
        vector<sf::ConvexShape> slices;
        double total = 0;
        for (double val : values)
            total += val;

        double startAngle = 0;
        for (size_t i = 0; i < values.size(); ++i)
        {
            double angle = 360.0 * (values[i] / total);
            int pointCount = 40;

            sf::ConvexShape slice;
            slice.setPointCount(pointCount + 2);
            slice.setFillColor(colors[i]);
            slice.setPoint(0, sf::Vector2f(cx, cy));

            for (int j = 0; j <= pointCount; ++j)
            {
                double theta = (startAngle + j * (angle / pointCount)) * 3.14159265 / 180.0;
                float x = cx + radius * cos(theta);
                float y = cy + radius * sin(theta);
                slice.setPoint(j + 1, sf::Vector2f(x, y));
            }

            slices.push_back(slice);
            startAngle += angle;
        }

        return slices;
    }
};

// ----------- EyeColorPredictor Class ------------------
class EyeColorPredictor
{
private:
    unordered_map<string, SNPData> snpDatabase;
    EyeColorOutput outputDisplay;

public:
    EyeColorPredictor() : outputDisplay(0, 0, 0)
    {
        loadDatabase();
    }

    void loadDatabase()
    {
        addSNP("rs12913832", 0.40, {{"AA", Score(0.8, 0.1, 0.1)}, {"AG", Score(0.5, 0.3, 0.2)}, {"GG", Score(0.1, 0.2, 0.7)}});
        addSNP("rs1800407", 0.15, {{"CC", Score(0.6, 0.3, 0.1)}, {"CT", Score(0.4, 0.4, 0.2)}, {"TT", Score(0.1, 0.3, 0.6)}});
        addSNP("rs12896399", 0.10, {{"GG", Score(0.7, 0.2, 0.1)}, {"GT", Score(0.5, 0.3, 0.2)}, {"TT", Score(0.2, 0.4, 0.4)}});
        addSNP("rs1393350", 0.10, {{"GG", Score(0.8, 0.1, 0.1)}, {"GA", Score(0.5, 0.3, 0.2)}, {"AA", Score(0.2, 0.3, 0.5)}});
        addSNP("rs16891982", 0.15, {{"CC", Score(0.7, 0.2, 0.1)}, {"CG", Score(0.4, 0.4, 0.2)}, {"GG", Score(0.1, 0.3, 0.6)}});
        addSNP("rs12203592", 0.10, {{"CC", Score(0.6, 0.3, 0.1)}, {"CT", Score(0.3, 0.4, 0.3)}, {"TT", Score(0.1, 0.3, 0.6)}});
    }

    void addSNP(const string &snp, double weight, const unordered_map<string, Score> &scores)
    {
        SNPData data;
        data.setWeight(weight);
        for (const auto &p : scores)
        {
            data.addGenotypeScore(p.first, p.second);
        }
        snpDatabase[snp] = data;
    }

    void calculateAndDisplay(const unordered_map<string, string> &maleSNPs,
                             const unordered_map<string, string> &femaleSNPs)
    {
        double totalBlue = 0.0, totalGreen = 0.0, totalBrown = 0.0, totalWeight = 0.0;

        for (const auto &entry : snpDatabase)
        {
            const string &snp = entry.first;
            const SNPData &snpData = entry.second;

            auto maleIt = maleSNPs.find(snp);
            auto femaleIt = femaleSNPs.find(snp);

            if (maleIt == maleSNPs.end() || femaleIt == femaleSNPs.end())
            {
                cout << "[!] Missing data for SNP: " << snp << endl;
                continue;
            }

            string childGenotype = getChildGenotype(maleIt->second, femaleIt->second);

            if (!snpData.hasGenotype(childGenotype))
            {
                cout << "[!] Unknown genotype: " << childGenotype << " for SNP " << snp << endl;
                continue;
            }

            Score score = snpData.getScore(childGenotype);
            double w = snpData.getWeight();

            totalBlue += score.getBlue() * w;
            totalGreen += score.getGreen() * w;
            totalBrown += score.getBrown() * w;
            totalWeight += w;
        }

        if (totalWeight > 0)
        {
            double bluePercent = (totalBlue / totalWeight) * 100;
            double greenPercent = (totalGreen / totalWeight) * 100;
            double brownPercent = (totalBrown / totalWeight) * 100;

            cout << "\n=== Eye Color Probabilities for Child ===\n";
            cout << "Blue  : " << bluePercent << " %\n";
            cout << "Green : " << greenPercent << " %\n";
            cout << "Brown : " << brownPercent << " %\n";

            outputDisplay = EyeColorOutput(bluePercent, greenPercent, brownPercent);
            outputDisplay.showOutput();
        }
        else
        {
            cout << "Not enough data to calculate probabilities.\n";
        }
    }

private:
    string getChildGenotype(string male, string female)
    {
        string g = "";
        g += male[0];
        g += female[0];
        sort(g.begin(), g.end());
        return g;
    }
};

// ----------- Main Program ------------------
int main()
{
    FileLoader fileLoader;
    DNAParser dnaParser;

    cout << "Select Father DNA file...\n";
    if (!fileLoader.selectFile())
    {
        cout << "No Father DNA file selected.\n";
        return 1;
    }
    string malePath = fileLoader.getFilePath();
    auto maleSNPs = dnaParser.parseFile(malePath);
    dnaParser.printSNPs(maleSNPs, "Father");

    cout << "Select Mother DNA file...\n";
    if (!fileLoader.selectFile())
    {
        cout << "No Mother DNA file selected.\n";
        return 1;
    }
    string femalePath = fileLoader.getFilePath();
    auto femaleSNPs = dnaParser.parseFile(femalePath);
    dnaParser.printSNPs(femaleSNPs, "Mother");

    EyeColorPredictor predictor;
    predictor.calculateAndDisplay(maleSNPs, femaleSNPs);

    return 0;
}