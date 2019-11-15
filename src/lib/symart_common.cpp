#include "symart_common.hpp"
#include "hyperbolic_group.hpp"


HyperbolicHelper::HyperbolicHelper( int fundamentalDomainFamilyIndex ):
  fdf{std::make_unique< fundamental_domain_family>( fundamentalDomainFamily( fundamentalDomainFamilyIndex ) )}
{
}


std::vector<int> HyperbolicHelper::rotation_point_counts() const
{
  return fdf->rotation_point_counts() ;
}

int HyperbolicHelper::rot180s() const
{
  return fdf->rot180s() ;
}

std::vector<std::vector<int>> HyperbolicHelper::mirror_point_counts() const
{
  return fdf->mirror_point_counts();
}

int HyperbolicHelper::loops() const
{
  return fdf->loops();
}

bool HyperbolicHelper::orientable() const
{
  return fdf->orientable();
}
