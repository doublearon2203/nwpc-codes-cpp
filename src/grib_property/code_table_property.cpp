#include "code_table_property.h"
#include "number_convert.h"

#include <fmt/format.h>

#include <stdexcept>

namespace grib_coder {

void CodeTableProperty::setTableDatabase(std::shared_ptr<GribTableDatabase> db) {
    table_database_ = db;
}

void CodeTableProperty::setTablesVersion(const std::string& version) {
    tables_version_ = version;
}

void CodeTableProperty::setLong(long value) {
    value_ = value;
}

long CodeTableProperty::getLong() {
    return value_;
}

void CodeTableProperty::setDouble(double value) {
    value_ = static_cast<long>(value);
}

double CodeTableProperty::getDouble() {
    return value_;
}

void CodeTableProperty::setString(const std::string& value) {
}

std::string CodeTableProperty::getString() {
    const auto default_value = fmt::format("{}", value_);

    const auto record_result = getTableRecord();
    if (!record_result.has_value()) {
        return default_value;
    }

    const auto record = record_result.value();

    if (record.abbreviation_ == record_unknown_value) {
        return record.title_;
    }

    return record.abbreviation_;
}

std::string CodeTableProperty::getTitle() {
    const auto default_value = fmt::format("{}", value_);

    const auto record_result = getTableRecord();
    if (!record_result.has_value()) {
        return default_value;
    }
    const auto record = record_result.value();

    return record.title_;
}

std::string CodeTableProperty::getAbbreviation() {
    const auto default_value = fmt::format("{}", value_);

    const auto record_result = getTableRecord();
    if (!record_result.has_value()) {
        return default_value;
    }
    const auto record = record_result.value();

    return record.abbreviation_;
}

std::string CodeTableProperty::getUnits() {
    const auto default_value = fmt::format("{}", value_);

    const auto record_result = getTableRecord();
    if (!record_result.has_value()) {
        return default_value;
    }
    const auto record = record_result.value();

    return record.units_;
}

void CodeTableProperty::setCodeTableId(const std::string& code_table_id) {
    code_table_id_ = code_table_id;
}

void CodeTableProperty::setOctetCount(size_t count) {
    octet_count_ = count;
}

bool CodeTableProperty::parse(std::vector<std::byte>::const_iterator& iterator, size_t count) {
    if (count == 1) {
        value_ = convert_bytes_to_uint8(&(*iterator));
    } else if (count == 2) {
        value_ = convert_bytes_to_uint16(&(*iterator));
    } else {
        throw std::runtime_error("count is not supported");
    }
    return true;
}

void CodeTableProperty::dump(const DumpConfig& dump_config) {
    fmt::print("{} [{} ({}) ]", getLong(), getString(), code_table_id_);
}

std::optional<GribTableRecord> CodeTableProperty::getTableRecord() {
    auto table = table_database_->getGribTable(tables_version_, code_table_id_);
    if (!table) {
        // std::cerr << "grib table (" << code_table_id_ << ") is not found." << std::endl;
        return std::optional<GribTableRecord>();
    }

    auto record = table->getRecord(value_);
    return record;
}

} // namespace grib_coder
