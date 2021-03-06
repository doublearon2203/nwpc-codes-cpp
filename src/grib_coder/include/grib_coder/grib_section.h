#pragma once

#include <grib_property/number_property.h>
#include <grib_property/grib_component.h>

#include <vector>
#include <unordered_map>
#include <cstdio>

namespace grib_coder {

class GribMessageHandler;

class GribSection : public GribComponent {
public:
    explicit GribSection(int section_number);
    GribSection(int section_number, long section_length);

    virtual ~GribSection() = default;

    GribSection(GribSection&&) = default;
    GribSection& operator= (GribSection&&) = default;

    GribSection(const GribSection&) = delete;

    // property
    void setLong(const std::string& key, long value) override;
    long getLong(const std::string& key) override;

    void setDouble(const std::string& key, double value) override;
    double getDouble(const std::string& key) override;

    void setString(const std::string& key, const std::string& value) override;
    std::string getString(const std::string& key) override;

    void setDoubleArray(const std::string& key, std::vector<double>& values) override;
    std::vector<double> getDoubleArray(const std::string& key) override;

    bool hasProperty(const std::string& key) override;

    GribProperty* getProperty(const std::string& name);

    void registerProperty(const std::string& name, GribProperty* property);
    void unregisterProperty(const std::string& name);

    // section length and number
    void setSectionLength(long length);
    long getSectionLength() const;

    long getByteCount() const override;

    int getSectionNumber() const;

    // parse, dump and pack
    virtual bool parseFile(std::FILE* file, bool header_only = false) = 0;

    bool decode(GribMessageHandler* handler) override;

    void dumpSection(GribMessageHandler* message_handler, std::size_t start_octec,
                     const DumpConfig& dump_config = DumpConfig{});

    bool encode(GribMessageHandler* handler) override;

    void pack(std::back_insert_iterator<std::vector<std::byte>>& iterator) override;

protected:
    // update section length using all components.
    virtual void updateSectionLength();

    // properties and templates which are encoded in the grib file.
    std::vector<std::unique_ptr<GribComponent>> components_;

    // including other properties which are not in grib file, such as computed properties.
    // template properties are also registered into section's property map.
    std::unordered_map<std::string, GribProperty*> property_map_;

    NumberProperty<uint8_t> section_number_;
    NumberProperty<uint32_t> section_length_;
};

GribProperty* get_property_from_section_list(
    const std::string& name,
    std::vector<std::shared_ptr<GribSection>>& section_list);
} // namespace grib_coder
