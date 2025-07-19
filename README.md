# Child eye color predictor from dna repor

This C++ application predicts a child's eye color based on parental DNA data using SNPs (Single Nucleotide Polymorphisms). It analyzes genetic markers associated with eye color inheritance, calculates probability scores, and visually presents the results using SFML (Simple and Fast Multimedia Library).

---

## 🚀 Features

- 🧬 **DNA File Loader** (uses Windows file dialog)
- 🔍 **SNP Extraction** from `.txt` DNA reports
- 📊 **Eye Color Probability Calculation** using genetic scoring model
- 🎨 **Visual Output**:
  - Pie chart showing color distribution
  - Colorized eye rendering
  - Predicted most likely eye color

## 📂 File Format Supported
- 23andMe-style `.txt` DNA files with rows like:
- rs12913832 15 28356859 AA


## 🧠 Target SNPs Used
- `rs12913832`
- `rs1800407`
- `rs12896399`
- `rs1393350`
- `rs16891982`
- `rs12203592`
## 🖥️ How to Run

### Requirements:
- Windows OS
- C++ Compiler (like MinGW, MSVC)
- [SFML Library](https://www.sfml-dev.org/)
- `eye.png` (eye image in the same folder)
- `arial.ttf` font file available in system or working directory

### Steps:
1. Build the program with SFML linked
2. Run the executable
3. Select the **Father DNA file** and then **Mother DNA file**
4. View prediction and visualization window
 ## 📚 How It Works

- Each SNP has a weight and associated scores for blue, green, and brown eye likelihood.
- Parent genotypes are combined to simulate a child’s genotype.
- Scores are aggregated to compute final eye color probabilities.
- The dominant predicted color is visualized with a tinted eye and pie chart.
  ## 🧪 Example Output

=== Eye Color Probabilities for Child ===
Blue : 45.0 %
Green : 25.0 %
Brown : 30.0 %
## 💡 Future Improvements

- Add support for drag-and-drop DNA files
- Export results to image or PDF
- Extend prediction to hair or skin color
