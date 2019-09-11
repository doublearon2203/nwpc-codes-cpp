#include "template_4_11.h"
#include "sections/grib_section_0.h"

#include <grib_property/number_convert.h>
#include <grib_property/grib_property_container.h>

#include <cassert>

namespace grib_coder {

Template_4_11::Template_4_11(int template_length):
    GribTemplate{template_length} {
    assert(template_length == 61 - 9);
    init();
}

bool Template_4_11::parse(std::vector<std::byte>& buffer) {
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
    scaled_value_of_second_fixed_surface_ = convert_bytes_to_uint32(&buffer[30], 4);

    type_of_ensemble_forecast_.setLong(convert_bytes_to_uint8(&buffer[34]));
    perturbation_number_ = convert_bytes_to_uint8(&buffer[35]);
    number_of_forecasts_in_ensemble_ = convert_bytes_to_uint8(&buffer[36]);

    year_of_end_of_overall_time_interval_ = convert_bytes_to_uint16(&buffer[37], 2);
    month_of_end_of_overall_time_interval_ = convert_bytes_to_uint8(&buffer[39]);
    day_of_end_of_overall_time_interval_ = convert_bytes_to_uint8(&buffer[40]);
    hour_of_end_of_overall_time_interval_ = convert_bytes_to_uint8(&buffer[41]);
    minute_of_end_of_overall_time_interval_ = convert_bytes_to_uint8(&buffer[42]);
    second_of_end_of_overall_time_interval_ = convert_bytes_to_uint8(&buffer[43]);
    number_of_time_range_ = convert_bytes_to_uint8(&buffer[44]);
    number_of_missing_statistical_process_ = convert_bytes_to_uint32(&buffer[45], 4);
    const auto type_of_statistical_processing = convert_bytes_to_uint8(&buffer[49]);
    type_of_statistical_processing_.setLong(type_of_statistical_processing);
    const auto type_of_time_increment = convert_bytes_to_uint8(&buffer[50]);
    type_of_time_increment_.setLong(type_of_time_increment);
    const auto indicator_of_unit_for_time_range = convert_bytes_to_uint8(&buffer[51]);
    indicator_of_unit_for_time_range_.setLong(indicator_of_unit_for_time_range);
    length_of_time_range_ = convert_bytes_to_uint32(&buffer[52], 4);
    const auto indicator_of_unit_for_time_increment = convert_bytes_to_uint8(&buffer[56]);
    indicator_of_unit_for_time_increment_.setLong(indicator_of_unit_for_time_increment);
    time_increment_ = convert_bytes_to_uint32(&buffer[57], 4);

    return true;
}

bool Template_4_11::decode(GribPropertyContainer* container) {
    const auto discipline = container->getLong("discipline");
    const auto category_table_id = fmt::format("4.1.{discipline}", fmt::arg("discipline", discipline));
    parameter_category_.setCodeTableId(category_table_id);

    const auto number_table_id = fmt::format("4.2.{discipline}.{category}",
                                             fmt::arg("discipline", discipline),
                                             fmt::arg("category", parameter_category_.getLong()));
    parameter_number_.setCodeTableId(number_table_id);

    level_.decode(container);
    type_of_level_.decode(container);
    step_range_.decode(container);

    return true;
}

void Template_4_11::registerProperty(std::shared_ptr<GribSection> section) {

    std::vector<std::tuple<std::string, GribProperty*>> properties_name{
        { "parameterCategory", &parameter_category_ },
        { "parameterNumber", &parameter_number_ },
        { "typeOfGeneratingProcess", &type_of_generating_process_ },
        { "backgroundProcess", &background_process_ },
        { "generatingProcessIdentifier", &generating_process_identifier_ },
        { "hoursAfterDataCutoff", &hours_after_data_cutoff_ },
        { "minutesAfterDataCutoff", &minutes_after_data_cutoff_ },
        { "indicatorOfUnitOfTimeRange", &indicator_of_unit_of_time_range_ },
        { "forecastTime", &forecast_time_ },
        { "typeOfFirstFixedSurface", &type_of_first_fixed_surface_ },
        { "scaleFactorOfFirstFixedSurface", &scale_factor_of_first_fixed_surface_ },
        { "scaledValueOfFirstFixedSurface", &scaled_value_of_first_fixed_surface_ },
        { "typeOfSecondFixedSurface", &type_of_second_fixed_surface_ },
        { "scaleFactorOfSecondFixedSurface", &scale_factor_of_second_fixed_surface_ },
        { "scaledValueOfSecondFixedSurface", &scaled_value_of_second_fixed_surface_ },

        {"yearOfEndOfOverallTimeInterval", &year_of_end_of_overall_time_interval_},
        {"monthOfEndOfOverallTimeInterval", &month_of_end_of_overall_time_interval_},
        {"dayOfEndOfOverallTimeInterval", &day_of_end_of_overall_time_interval_},
        {"hourOfEndOfOverallTimeInterval", &hour_of_end_of_overall_time_interval_},
        {"minuteOfEndOfOverallTimeInterval", &minute_of_end_of_overall_time_interval_},
        {"secondOfEndOfOverallTimeInterval", &second_of_end_of_overall_time_interval_},
        {"numberOfTimeRange", &number_of_time_range_},
        {"numberOfMissingInStatisticalProcess", &number_of_missing_statistical_process_},
        {"typeOfStatisticalProcessing", &type_of_statistical_processing_},
        {"typeOfTimeIncrement", &type_of_time_increment_},
        {"indicatorOfUnitForTimeRange", &indicator_of_unit_for_time_range_},
        {"lengthOfTimeRange", &length_of_time_range_},
        {"indicatorOfUnitForTimeIncrement", &indicator_of_unit_for_time_increment_},
        {"timeIncrement", &time_increment_},

        { "level", &level_ },
        { "typeOfLevel", &type_of_level_ },
        { "stepRange", &step_range_ },
    };
    for (const auto& item : properties_name) {
        section->registerProperty(std::get<0>(item), std::get<1>(item));
    }
}

void Template_4_11::init() {
    std::vector<std::tuple<CodeTableProperty*, std::string>> tables_id{
        {&type_of_generating_process_, "4.3"},
        {&indicator_of_unit_of_time_range_, "4.4"},
        {&type_of_first_fixed_surface_, "4.5"},
        {&type_of_second_fixed_surface_, "4.5"},

        {&type_of_ensemble_forecast_, "4.6"},

        {&type_of_statistical_processing_, "4.10"},
        {&type_of_time_increment_, "4.11"},
        {&indicator_of_unit_for_time_range_, "4.4"},
        {&indicator_of_unit_for_time_increment_, "4.4"}
    };
    for (const auto& item : tables_id) {
        std::get<0>(item)->setCodeTableId(std::get<1>(item));
    }
}
} // namespace grib_coder
