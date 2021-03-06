#include <grib_coder/grib_message_handler.h>

#include <grib_coder/sections/grib_section_0.h>
#include <grib_coder/sections/grib_section_1.h>
#include <grib_coder/sections/grib_section_3.h>
#include <grib_coder/sections/grib_section_4.h>
#include <grib_coder/sections/grib_section_5.h>
#include <grib_coder/sections/grib_section_6.h>
#include <grib_coder/sections/grib_section_7.h>
#include <grib_coder/sections/grib_section_8.h>

#include <grib_property/number_convert.h>
#include <grib_property/grib_table_database.h>

#include <fmt/format.h>

#include <stdexcept>

namespace grib_coder {

GribMessageHandler::GribMessageHandler(std::shared_ptr<GribTableDatabase>& db, bool header_only):
    header_only_{header_only},
    table_database_{db} {
    property_map_["count"] = &count_;
    property_map_["offset"] = &offset_;
}

GribMessageHandler::~GribMessageHandler() {}

void GribMessageHandler::setCount(long count) {
    count_ = count;
}

bool GribMessageHandler::parseFile(std::FILE* file) {
    const auto start_pos = std::ftell(file);
    offset_ = start_pos;
    auto section_0 = std::make_shared<GribSection0>();
    auto result = section_0->parseFile(file);
    if (!result) {
        return false;
    }
    section_list_.push_back(section_0);

    auto current_pos = std::ftell(file);

    // check file end
    const auto section8_start_pos = start_pos + section_0->getProperty("totalLength")->getLong() - 4;

    while (current_pos < section8_start_pos) {
        parseNextSection(file);
        current_pos = std::ftell(file);
    }

    if (current_pos != section8_start_pos) {
        return false;
    }

    auto section_8 = std::make_shared<GribSection8>();
    result = section_8->parseFile(file);

    if (!result) {
        return false;
    }

    section_list_.push_back(section_8);

    return true;
}

bool GribMessageHandler::decodeValues() {
    for (auto& section : section_list_) {
        if (section->getSectionNumber() == 7) {
            auto section7 = std::static_pointer_cast<GribSection7>(section);
            if (!section7->decodeValues(this)) {
                return false;
            }
        }
    }
    return true;
}

void GribMessageHandler::setLong(const std::string& key, long value) {
    auto property = getProperty(key);
    if (property == nullptr) {
        throw std::runtime_error("key is not found");
    }
    property->setLong(value);
}

long GribMessageHandler::getLong(const std::string& key) {
    auto property = getProperty(key);
    if (property == nullptr) {
        throw std::runtime_error("key is not found");
    }
    return property->getLong();
}

void GribMessageHandler::setDouble(const std::string& key, double value) {
    auto property = getProperty(key);
    if (property == nullptr) {
        throw std::runtime_error("key is not found");
    }
    property->setDouble(value);
}

double GribMessageHandler::getDouble(const std::string& key) {
    auto property = getProperty(key);
    if (property == nullptr) {
        throw std::runtime_error("key is not found");
    }
    return property->getDouble();
}

void GribMessageHandler::setString(const std::string& key, const std::string& value) {
    auto property = getProperty(key);
    if (property == nullptr) {
        throw std::runtime_error("key is not found");
    }
    auto code_table_property = dynamic_cast<CodeTableProperty*>(property);
    if (code_table_property) {
        // set grib2 table database
        const auto table_version = getLong("tablesVersion");
        code_table_property->setTableDatabase(table_database_);
        code_table_property->setTablesVersion(fmt::format("{}", table_version));
    }
    property->setString(value);
}

std::string GribMessageHandler::getString(const std::string& key) {
    auto property = getProperty(key);
    if (property == nullptr) {
        throw std::runtime_error("key is not found");
    }
    auto code_table_property = dynamic_cast<CodeTableProperty*>(property);
    if (code_table_property) {
        // set grib2 table database
        const auto table_version = getLong("tablesVersion");
        code_table_property->setTableDatabase(table_database_);
        code_table_property->setTablesVersion(fmt::format("{}", table_version));
    }
    return property->getString();
}

bool GribMessageHandler::hasProperty(const std::string& key) {
    const auto property = getProperty(key);
    return property != nullptr;
}


void GribMessageHandler::dump(const DumpConfig& dump_config) {
    for (const auto& section : section_list_) {
        section->dumpSection(this, 1, dump_config);
    }
}

bool GribMessageHandler::encodeValues() {
    for (auto& section : section_list_) {
        if (section->getSectionNumber() == 7) {
            auto section7 = std::static_pointer_cast<GribSection7>(section);
            if (!section7->encodeValues(this)) {
                return false;
            }
        }
    }
    return true;
}

bool GribMessageHandler::packFile(std::FILE* file) {
    for(auto iter=std::rbegin(section_list_); iter !=std::rend(section_list_); ++iter ) {
        (*iter)->encode(this);
    }

    std::vector<std::byte> bytes;
    auto iterator = std::back_inserter(bytes);
    for (const auto& section : section_list_) {
        section->pack(iterator);
    }

    std::fwrite(&bytes[0], 1, bytes.size(), file);

    return true;
}

long GribMessageHandler::calculateTotalLength() const {
    long total_length = 0;
    for (auto& section : section_list_) {
        total_length += section->getByteCount();
    }
    return total_length;
}

bool GribMessageHandler::parseNextSection(std::FILE* file) {
    std::byte buffer[5];
    const auto result = std::fread(buffer, 1, 5, file);
    if (result != 5) {
        return false;
    }
    const auto section_length = convert_bytes_to_number<uint32_t>(buffer);
    const auto section_number = convert_bytes_to_number<uint8_t>(&buffer[4]);

    std::shared_ptr<GribSection> section;

    if (section_number == 1) {
        section = std::make_shared<GribSection1>(section_length);
    } else if (section_number == 2) {
        throw std::runtime_error("section 2 is not supported");
    } else if (section_number == 3) {
        section = std::make_shared<GribSection3>(section_length);
    } else if (section_number == 4) {
        section = std::make_shared<GribSection4>(section_length);
    } else if (section_number == 5) {
        section = std::make_shared<GribSection5>(section_length);
    } else if (section_number == 6) {
        section = std::make_shared<GribSection6>(section_length);
    } else if (section_number == 7) {
        section = std::make_shared<GribSection7>(section_length);
    } else {
        throw std::runtime_error(fmt::format("section number is not supported:{}", section_number));
    }

    // NOTE: where to put this line
    section_list_.push_back(section);

    auto flag = section->parseFile(file, header_only_);
    if (!flag) {
        return false;
    }

    flag = section->decode(this);
    if (!flag) {
        return false;
    }

    if (section_number == 6 && !header_only_) {
        auto section6 = std::static_pointer_cast<GribSection6>(section);
        if (!section6->decodeValues(this)) {
            return false;
        }
    }

    if (section_number == 7 && !header_only_) {
        auto section7 = std::static_pointer_cast<GribSection7>(section);
        if (!section7->decodeValues(this)) {
            return false;
        }
    }

    return true;
}

auto GribMessageHandler::getSection(int section_number, size_t begin_pos) {
    std::shared_ptr<GribSection> section;
    for (auto iter = section_list_.begin() + begin_pos; iter != section_list_.end(); ++iter) {
        auto s = *iter;
        if (s->getSectionLength() == section_number) {
            return s;
        }
    }
    return section;
}

GribProperty* GribMessageHandler::getProperty(const std::string& name) {
    for (auto& item : property_map_) {
        if (std::get<0>(item) == name) {
            return std::get<1>(item);
        }
    }

    for (auto& section : section_list_) {
        auto p = section->getProperty(name);
        if (p != nullptr) {
            return p;
        }
    }

    return nullptr;
}

} // namespace grib_coder
