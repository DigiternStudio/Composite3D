#ifndef CM3D_VECTOR3_HPP
#define CM3D_VECTOR3_HPP

#include "Scalar.hpp"

namespace cm3d
{
    struct Vector3
	{
		Scalar x;
		Scalar y;
		Scalar z;

	/* ================================================ */
		
		static constexpr int length() {return 3;}
		
		constexpr Scalar & operator[](int i);
		constexpr Scalar const& operator[](int i) const;

	/* ================================================ */

		Vector3() = default;
		Vector3(Vector3 const& v);

		constexpr explicit Vector3(Scalar sc);
		constexpr Vector3(Scalar x, Scalar y, Scalar z);
		
		template<typename T>
		constexpr explicit Vector3(glm::vec<3, T, glm::defaultp> v);

	/* ================================================ */

		explicit operator glm::dvec3() const;
		explicit operator glm::vec3() const;

		operator bool() const;

	/* ================================================ */

		constexpr Vector3 & operator+=(Vector3 const& v);

		constexpr Vector3 & operator-=(Vector3 const& v);

		constexpr Vector3 & operator*=(Scalar const& v);

		constexpr Vector3 & operator*=(Vector3 const& v);

		constexpr Vector3 & operator/=(Scalar const& v);

		constexpr Vector3 & operator/=(Vector3 const& v);

	/* ================================================ */

		constexpr Vector3 & operator=(Vector3 const& v);

		constexpr Vector3 & operator+=(Vector3 const& v);

		constexpr Vector3 & operator-=(Vector3 const& v);

		constexpr Vector3 & operator*=(Scalar const& v);

		constexpr Vector3 & operator*=(Vector3 const& v);

		constexpr Vector3 & operator/=(Scalar const& v);

		constexpr Vector3 & operator/=(Vector3 const& v);
	};
}

// s#include "Vector3.inl"

#endif // CM3D_VECTOR3_HPP