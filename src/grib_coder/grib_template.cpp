#include "grib_template.h"

namespace grib_coder {

GribTemplate::GribTemplate(int template_length):
    template_length_{ template_length }
{
}

GribTemplate::~GribTemplate()
{
}

bool GribTemplate::decode(GribPropertyContainer* container)
{
    return true;
}

} // namespace grib_coder