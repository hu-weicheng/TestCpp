#include <iostream>
#include <vector>
#include <complex>
#include <cmath>
#include <fstream>

#define M_PI 3.141592654

class SpectrumAnalyzer {
private:
    int sampleRate;
    int fftSize;

public:
    SpectrumAnalyzer(int sr, int size) : sampleRate(sr), fftSize(size) {}

    // 计算幅度谱
    std::vector<double> computeMagnitudeSpectrum(const std::vector<double>& signal) {
        if (signal.size() != fftSize) {
            throw std::runtime_error("信号长度必须等于FFT点数");
        }

        // 1. 应用汉宁窗减少频谱泄漏
        std::vector<double> windowed = signal;
        for (int i = 0; i < fftSize; i++) {
            double window = 0.5 * (1 - cos(2 * M_PI * i / (fftSize - 1)));
            windowed[i] *= window;
        }

        // 2. 执行FFT（这里用简单的DFT演示，实际用FFTW）
        std::vector<std::complex<double>> spectrum(fftSize);
        for (int k = 0; k < fftSize; k++) {
            std::complex<double> sum(0, 0);
            for (int n = 0; n < fftSize; n++) {
                double angle = -2 * M_PI * k * n / fftSize;
                sum += windowed[n] * std::complex<double>(cos(angle), sin(angle));
            }
            spectrum[k] = sum;
        }

        // 3. 计算幅度并归一化
        std::vector<double> magnitude(fftSize / 2);
        for (int k = 0; k < fftSize / 2; k++) {
            magnitude[k] = std::abs(spectrum[k]) * 2.0 / fftSize;
        }
        magnitude[0] /= 2; // DC分量

        return magnitude;
    }

    // 找到峰值频率
    double findPeakFrequency(const std::vector<double>& magnitude) {
        int peakBin = 0;
        double maxMag = 0;

        for (int k = 0; k < magnitude.size(); k++) {
            if (magnitude[k] > maxMag) {
                maxMag = magnitude[k];
                peakBin = k;
            }
        }

        return (double)peakBin * sampleRate / fftSize;
    }
};

int testFFT() 
{
    // 参数设置
    int sampleRate = 1000;  // 1kHz采样率
    int fftSize = 1024;     // 1024点FFT

    // 生成测试信号：100Hz + 200Hz正弦波
    std::vector<double> signal(fftSize);
    for (int i = 0; i < fftSize; i++) {
        double t = (double)i / sampleRate;
        signal[i] = 0.8 * sin(2 * M_PI * 100 * t) +  // 100Hz
            0.3 * sin(2 * M_PI * 200 * t);   // 200Hz
    }

    // 分析频谱
    SpectrumAnalyzer analyzer(sampleRate, fftSize);
    auto magnitude = analyzer.computeMagnitudeSpectrum(signal);
    double peakFreq = analyzer.findPeakFrequency(magnitude);

    std::cout << "峰值频率: " << peakFreq << " Hz" << std::endl;

    // 输出到文件（可用Excel或绘图工具查看）
    std::ofstream out("spectrum.csv");
    out << "频率(Hz),幅度\n";
    for (int k = 0; k < magnitude.size(); k++) {
        double freq = (double)k * sampleRate / fftSize;
        out << freq << "," << magnitude[k] << "\n";
    }
    out.close();

    std::cout << "频谱数据已保存到 spectrum.csv" << std::endl;

    return 0;
}