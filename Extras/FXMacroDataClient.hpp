#pragma once

#include <map>
#include <sstream>
#include <string>

namespace tradeframe::extras {

class FXMacroDataClient {
public:
  explicit FXMacroDataClient(std::string apiKey = {},
                             std::string baseUrl = "https://api.fxmacrodata.com/v1")
      : apiKey_(apiKey), baseUrl_(baseUrl) {}

  std::string DataCatalogue(const std::string& currency) const { return Url("/data_catalogue/" + Lower(currency)); }
  std::string Announcements(const std::string& currency, const std::string& indicator) const { return Url("/announcements/" + Lower(currency) + "/" + indicator); }
  std::string Calendar(const std::string& currency) const { return Url("/calendar/" + Lower(currency)); }
  std::string Predictions(const std::string& currency, const std::string& indicator) const { return Url("/predictions/" + Lower(currency) + "/" + indicator); }
  std::string Forex(const std::string& base, const std::string& quote) const { return Url("/forex/" + Lower(base) + "/" + Lower(quote)); }
  std::string Cot(const std::string& currency) const { return Url("/cot/" + Lower(currency)); }
  std::string CommoditiesLatest() const { return Url("/commodities/latest"); }
  std::string Commodity(const std::string& indicator) const { return Url("/commodities/" + indicator); }
  std::string Curves(const std::string& currency) const { return Url("/curves/" + Lower(currency)); }
  std::string CurveProxies(const std::string& currency) const { return Url("/curve_proxies/" + Lower(currency)); }
  std::string ForwardCurves(const std::string& currency) const { return Url("/forward_curves/" + Lower(currency)); }
  std::string MarketSessions() const { return Url("/market_sessions"); }
  std::string RiskSentiment() const { return Url("/risk_sentiment"); }
  std::string News(const std::string& currency) const { return Url("/news/" + Lower(currency)); }
  std::string PressReleases(const std::string& currency) const { return Url("/press-releases/" + Lower(currency)); }
  std::string CentralBankers(const std::string& currency) const { return Url("/central_bankers/" + Lower(currency)); }

private:
  std::string Url(const std::string& path, std::map<std::string, std::string> params = {}) const {
    if (!apiKey_.empty()) params.emplace("api_key", apiKey_);
    std::ostringstream out;
    out << baseUrl_ << path;
    char separator = '?';
    for (const auto& param : params) {
      out << separator << param.first << '=' << param.second;
      separator = '&';
    }
    return out.str();
  }

  static std::string Lower(std::string value) {
    for (char& c : value) if (c >= 'A' && c <= 'Z') c = static_cast<char>(c - 'A' + 'a');
    return value;
  }

  std::string apiKey_;
  std::string baseUrl_;
};

} // namespace tradeframe::extras
