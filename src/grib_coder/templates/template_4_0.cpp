#include "template_4_0.h"
#include "sections/grib_section_0.h"

#include "number_convert.h"

namespace grib_coder {

Template_4_0::Template_4_0(int template_length):
    GribTemplate{ template_length }
{
    init();
}

Template_4_0::~Template_4_0()
{
}

bool Template_4_0::parse(std::vector<unsigned char>& buffer)
{
    const auto parameter_category = convert_bytes_to_uint8(&buffer[9]);
    parameter_category_.setLong(parameter_category);
    const auto parameter_number = convert_bytes_to_uint8(&buffer[10]);
    parameter_number_.setLong(parameter_number);
    const auto type_of_generating_process = convert_bytes_to_uint8(&buffer[11]);
    type_of_generating_process_.setLong(type_of_generating_process);
    background_process_ = convert_bytes_to_uint8(&buffer[12]);
    generating_process_identifier_ = convert_bytes_to_uint8(&buffer[13]);
    hours_after_data_cutoff_ = convert_bytes_to_uint16(&buffer[14], 2);
    minutes_after_data_cutoff_ = convert_bytes_to_uint8(&buffer[16]);
    const auto indicator_of_unit_of_time_range = convert_bytes_to_uint8(&buffer[17]);
    indicator_of_unit_of_time_range_.setLong(indicator_of_unit_of_time_range);
    forecast_time_ = convert_bytes_to_int32(&buffer[18], 4);
    const auto type_of_first_fixed_surface = convert_bytes_to_uint8(&buffer[22]);
    type_of_first_fixed_surface_.setLong(type_of_first_fixed_surface);
    scale_factor_of_first_fixed_surface_ = convert_bytes_to_int8(&buffer[23]);
    scaled_value_of_first_fixed_surface_ = convert_bytes_to_uint32(&buffer[24], 4);
    const auto type_of_second_fixed_surface = convert_bytes_to_uint8(&buffer[28]);
    type_of_second_fixed_surface_.setLong(type_of_second_fixed_surface);
    scale_factor_of_second_fixed_surface_ = convert_bytes_to_int8(&buffer[29]);
    scaled_value_of_second_fixed_surface_ = convert_bytes_to_uint32(&buffer[30]);

    return true;
}

bool Template_4_0::decode(GribPropertyContainer* container)
{
    const auto discipline = container->getLong("discipline");
    const auto category_table_id = fmt::format("4.1.{discipline}", fmt::arg("discipline", discipline));
    parameter_category_.setCodeTableId(category_table_id);

    const auto number_table_id = fmt::format("4.2.{discipline}.{category}",
        fmt::arg("discipline", discipline),
        fmt::arg("category", parameter_category_.getLong()));
    parameter_number_.setCodeTableId(number_table_id);

    return true;
}

void Template_4_0::registerProperty(std::shared_ptr<GribSection> section)
{
    section->registerProperty("parameterCategory", &parameter_category_);
    section->registerProperty("parameterNumber", &parameter_number_);
    section->registerProperty("typeOfGeneratingProcess", &type_of_generating_process_);
    section->registerProperty("backgroundProcess", &background_process_);
    section->registerProperty("generatingProcessIdentifier", &generating_process_identifier_);
    section->registerProperty("hoursAfterDataCutoff", &hours_after_data_cutoff_);
    section->registerProperty("minutesAfterDataCutoff", &minutes_after_data_cutoff_);
    section->registerProperty("indicatorOfUnitOfTimeRange", &indicator_of_unit_of_time_range_);
    section->registerProperty("forecastTime", &forecast_time_);
    section->registerProperty("typeOfFirstFixedSurface", &type_of_first_fixed_surface_);
    section->registerProperty("scaleFactorOfFirstFixedSurface", &scale_factor_of_first_fixed_surface_);
    section->registerProperty("scaledValueOfFirstFixedSurface", &scaled_value_of_first_fixed_surface_);
    section->registerProperty("typeOfSecondFixedSurface", &type_of_second_fixed_surface_);
    section->registerProperty("scaleFactorOfSecondFixedSurface", &scale_factor_of_second_fixed_surface_);
    section->registerProperty("scaledValueOfSecondFixedSurface", &scaled_value_of_second_fixed_surface_);
}

void Template_4_0::init()
{
    type_of_generating_process_.setCodeTableId("4.3");
    indicator_of_unit_of_time_range_.setCodeTableId("4.4");
    type_of_first_fixed_surface_.setCodeTableId("4.5");
    type_of_second_fixed_surface_.setCodeTableId("4.5");
}
} // namespace grib_coder