#pragma once

#include <grib_coder/grib_section.h>
#include <grib_coder/template_code_table_property.h>

namespace grib_coder {

class TemplateComponent;

class GribSection4 final: public GribSection {
public:
    GribSection4();
    explicit GribSection4(int section_length);

    bool parseFile(std::FILE* file, bool header_only = false) override;

    bool decode(GribMessageHandler* container) override;

private:
    void init();

    // generate production template. used in TemplateCodeTableProperty.
    void generateProductionTemplate(TemplateComponent* template_component);

    NumberProperty<uint16_t> nv_;
    TemplateCodeTableProperty product_definition_template_number_;
};

} // namespace grib_coder
