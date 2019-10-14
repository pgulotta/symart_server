#pragma once

#include <vector>
#include <memory>


enum class flip_type { ALL, ALTERNATING, RANDOM };

enum class projtype {POINCARE, KLEIN};

enum class symgroup {CM, CMM, P1, P2, P3, P31M, P3M1,
                     P4, P4G, P4M, P6, P6M, PG, PGG, PM,
                     PMG, PMM
                    };

const int INVALID_VALUE = -1;

class fundamental_domain_family;

class HyperbolicHelper
{
public:
  HyperbolicHelper( int fundamentalDomainFamilyIndex );
  std::vector<int> rotation_point_counts() const;
  int rot180s() const;
  std::vector<std::vector<int>> mirror_point_counts() const;
  int loops() const;
  bool orientable() const;

private:
  std::shared_ptr< fundamental_domain_family> fdf;

};


