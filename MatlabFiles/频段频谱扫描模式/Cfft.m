function [FrequecnyDomainDataArray] = Cfft(TimeDomainDataArray,ArraySize)
    FreqRetTemp = abs(fft(TimeDomainDataArray,ArraySize));
    FreqRetTempMax = max(FreqRetTemp);
    FrequecnyDomainDataArray = 20*log10(FreqRetTemp/FreqRetTempMax+0.0001)
    return;
end