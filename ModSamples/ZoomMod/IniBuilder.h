#ifndef INI_BUILDER_H
#define INI_BUILDER_H

class IniBuilder {
    std::map<std::string, std::string> data;
    std::string configPath;

public:
	IniBuilder() = default;
    IniBuilder(std::string _configPath) {
		configPath = _configPath;
		if (file.doesClientPathExist(configPath)) {
			Console::Log("ZoomMod", "Loading config from %s", configPath.c_str());
			std::string content = file.readFile(configPath);
			Put(content);
		}
        else {
            Console::Log("ZoomMod", "Config file %s not found, creating new one", configPath.c_str());
			file.writeFile(configPath, str());
        }
    }

public:
    void add(const std::string& key, const std::string& value) {
        data[key] = value;
		file.writeFile(configPath, str());
    }

    std::optional<std::string> Get(const std::string& key) const {
        auto it = data.find(key);
        if (it == data.end()) return std::nullopt;
        return it->second;
    }

    template<typename T>
    T GetOrDefault(const std::string& key, T def) {
        auto val = Get(key);
        if (!val.has_value()) {
            if constexpr (std::is_same_v<T, std::string>)
                data[key] = def;
            else
                data[key] = std::to_string(def);
            file.writeFile(configPath, str());
            return def;
        }
        if constexpr (std::is_same_v<T, std::string>) {
            return val.value();
        }
        else {
            std::istringstream ss(val.value());
            T out;
            ss >> out;
            if (ss.fail()) {
                data[key] = std::to_string(def);
                file.writeFile(configPath, str());
                return def;
            }
            return out;
        }
    }

    std::string str() const {
        std::ostringstream out;
        for (const auto& [key, value] : data)
            out << key << "=" << value << "\n";
        return out.str();
    }

    void Put(const std::string& content) {
        data.clear();
        std::istringstream in(content);
        std::string line;
        while (std::getline(in, line)) {
            if (line.empty() || line[0] == ';' || line[0] == '#' || (line.front() == '[' && line.back() == ']')) continue;
            auto eq = line.find('=');
            if (eq == std::string::npos) continue;
            std::string key = line.substr(0, eq);
            std::string value = line.substr(eq + 1);
            data[key] = value;
        }
    }
};

#endif // !INI_BUILDER_H
