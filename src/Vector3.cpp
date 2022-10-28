#include "Vector3.hpp"

namespace cm3d
{
    constexpr Scalar & Vector3::operator[](int i)
	{
		return	i == 2 ? z :
				i == 1 ? y :
				x;
	}
	constexpr Scalar const& Vector3::operator[](int i) const
	{
		return	i == 2 ? z :
				i == 1 ? y :
				x;
	}

	/* ============================================ */

	Vector3::Vector3(Vector3 const& v)
	{
		
	}

	constexpr explicit Vector3::Vector3(Scalar sc);
	constexpr Vector3::Vector3(Scalar x, Scalar y, Scalar z);
	
	template<typename T>
	constexpr explicit Vector3::Vector3(glm::vec<3, T, glm::defaultp> v);

	/* ============================================ */

	explicit operator Vector3::glm::dvec3() const;
	explicit operator Vector3::glm::vec3() const;

	operator Vector3::bool() const;

	/* ============================================ */

	constexpr Vector3 & Vector3::operator=(Vector3 const& v);

	constexpr Vector3 & Vector3::operator+=(Scalar const& v);

	constexpr Vector3 & Vector3::operator+=(Vector3 const& v);

	constexpr Vector3 & Vector3::operator-=(Scalar const& v);

	constexpr Vector3 & Vector3::operator-=(Vector3 const& v);

	constexpr Vector3 & Vector3::operator*=(Scalar const& v);

	constexpr Vector3 & Vector3::operator*=(Vector3 const& v);

	constexpr Vector3 & Vector3::operator/=(Scalar const& v);

	constexpr Vector3 & Vector3::operator/=(Vector3 const& v);
}

