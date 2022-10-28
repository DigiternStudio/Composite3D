#ifndef CM3D_SCALAR_HPP
#define CM3D_SCALAR_HPP

#include "BasicTypes.h"
#include "/Libs/glm/glm/gtc/type_ptr.hpp"

namespace cm3d
{
    struct Scalar
	{
		int64_t lv; // linear value (-2^63; 2^63)
		sReal rv; // clarification (-1.0; 1.0)

	/* ============================================== */

		Scalar() = default;

		constexpr Scalar(int64_t lv, sReal rv);
		inline Scalar(sReal rv);

		constexpr operator sReal();

	/* ============================================== */

		constexpr bool operator==(Scalar const& sc);
        constexpr bool operator!=(Scalar const& sc);

        constexpr bool operator>(Scalar const& sc);
        constexpr bool operator>=(Scalar const& sc);

        constexpr bool operator<(Scalar const& sc);
        constexpr bool operator<=(Scalar const& sc);

	/* ============================================== */

		constexpr Scalar operator+(Scalar sc);
		constexpr Scalar operator-(Scalar sc);
		constexpr Scalar operator*(sReal sc);
		constexpr Scalar operator/(sReal sc);

	/* ============================================== */

		constexpr Scalar & operator=(Scalar sc);

		constexpr Scalar & operator+=(Scalar sc);

		constexpr Scalar & operator-=(Scalar sc);

        constexpr Scalar & operator*=(sReal sc);
        constexpr Scalar & operator*=(int64_t sc);

        constexpr Scalar & operator/=(sReal sc);
        constexpr Scalar & operator/=(int64_t sc);
	};
}

#include "Scalar.inl"

#endif // CM3D_SCALAR_HPP