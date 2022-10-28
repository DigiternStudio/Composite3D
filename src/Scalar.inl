// #include "Scalar.hpp"
#include <cmath>

namespace cm3d
{
	constexpr Scalar::Scalar(int64_t lv, sReal rv) :
		lv(lv), rv(rv) {}
	
	inline Scalar::Scalar(sReal rv) {
		this->lv = static_cast<int64_t>(rv);
		this->rv = rv - static_cast<sReal>(lv);
	}
	
	constexpr Scalar::operator sReal() {
		return static_cast<sReal>(lv) + rv;
	}
	
/* ============================================== */

	constexpr bool Scalar::operator==(Scalar const& sc) {
		return lv == sc.lv && rv == sc.rv;
	}
	constexpr bool Scalar::operator!=(Scalar const& sc) {
		return lv != sc.lv || rv != sc.rv;
	}

	constexpr bool Scalar::operator>(Scalar const& sc) {
		return lv > sc.lv ||
			(lv == sc.lv && rv > sc.rv);
	}
	constexpr bool Scalar::operator>=(Scalar const& sc) {
		return lv > sc.lv ||
			(lv == sc.lv && rv >= sc.rv);
	}

	constexpr bool Scalar::operator<(Scalar const& sc) {
		return lv < sc.lv ||
			(lv == sc.lv && rv < sc.rv);
	}
	constexpr bool Scalar::operator<=(Scalar const& sc) {
		return lv < sc.lv ||
			(lv == sc.lv && rv <= sc.rv);
	}

/* ============================================== */

	constexpr Scalar Scalar::operator+(Scalar sc) {
		Scalar nsc = {lv + sc.lv, rv + sc.rv};
		if (std::abs(nsc.rv) >= static_cast<sReal>(1.0)) {
			int sb = std::signbit(nsc.rv);
			sb <<= 1; sb -= 1;
			nsc.lv -= sb;
			nsc.rv += static_cast<sReal>(sb);
		}
		return nsc;
	}
	constexpr Scalar Scalar::operator-(Scalar sc) {
		Scalar nsc = {lv - sc.lv, rv - sc.rv};
		if (std::abs(nsc.rv) >= static_cast<sReal>(1.0)) {
			int sb = std::signbit(nsc.rv);
			sb <<= 1; sb -= 1;
			nsc.lv -= sb;
			nsc.rv += static_cast<sReal>(sb);
		}
		return nsc;
	}
	constexpr Scalar Scalar::operator*(sReal sc) {
		sLongReal val = static_cast<sLongReal>(lv) + static_cast<sLongReal>(rv);
		val *= sc;

		int64_t int_part = static_cast<int64_t>(val);
		sReal diff = val - static_cast<sReal>(int_part);

		Scalar nsc = {int_part, diff};
		return nsc;
	}
	constexpr Scalar Scalar::operator/(sReal sc) {
		sLongReal val = static_cast<sLongReal>(lv) + static_cast<sLongReal>(rv);
		val *= sc;

		int64_t int_part = static_cast<int64_t>(val);
		sReal diff = val - static_cast<sReal>(int_part);

		Scalar nsc = {int_part, diff};
		return nsc;
	}

/* ============================================== */

	constexpr Scalar & Scalar::operator=(Scalar sc) {
		*this = sc;
		return *this;
	}

	constexpr Scalar & Scalar::operator+=(Scalar sc) {
		lv += sc.lv;
		rv += sc.rv;
		if (std::abs(rv) >= static_cast<sReal>(1.0)) {
			int sb = std::signbit(rv);
			sb <<= 1; sb -= 1;
			lv -= sb;
			rv += static_cast<sReal>(sb);
		}
		return *this;
	}

	constexpr Scalar & Scalar::operator-=(Scalar sc) {
		lv -= sc.lv;
		rv -= sc.rv;
		if (std::abs(rv) >= static_cast<sReal>(1.0)) {
			int sb = std::signbit(rv);
			sb <<= 1; sb -= 1;
			lv -= sb;
			rv += static_cast<sReal>(sb);
		}
		return *this;
	}

	constexpr Scalar & Scalar::operator*=(sReal sc) {
		sLongReal val = static_cast<sLongReal>(lv) + static_cast<sLongReal>(rv);
		val *= sc;

		int64_t int_part = static_cast<int64_t>(val);
		sReal diff = val - static_cast<sReal>(int_part);

		lv = int_part;
		rv = diff;
		return *this;
	}
	constexpr Scalar & Scalar::operator*=(int64_t sc) {
		sLongReal val = static_cast<sLongReal>(lv) + static_cast<sLongReal>(rv);
		val *= sc;

		int64_t int_part = static_cast<int64_t>(val);
		sReal diff = val - static_cast<sReal>(int_part);

		lv = int_part;
		rv = diff;
		return *this;
	}

	constexpr Scalar & Scalar::operator/=(sReal sc) {
		sLongReal val = static_cast<sLongReal>(lv) + static_cast<sLongReal>(rv);
		val /= sc;

		int64_t int_part = static_cast<int64_t>(val);
		sReal diff = val - static_cast<sReal>(int_part);

		lv = int_part;
		rv = diff;
		return *this;
	}
	constexpr Scalar & Scalar::operator/=(int64_t sc) {
		sLongReal val = static_cast<sLongReal>(lv) + static_cast<sLongReal>(rv);
		val /= sc;

		int64_t int_part = static_cast<int64_t>(val);
		sReal diff = val - static_cast<sReal>(int_part);

		lv = int_part;
		rv = diff;
		return *this;
	}
}