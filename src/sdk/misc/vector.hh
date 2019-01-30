#pragma once

#include <algorithm>
#include <ostream>
#include <assert.h>
#define valid_yes( _v ) 0

typedef float v_matrix[ 3 ][ 4 ];
namespace sdk {

    class vec2 {
    public:
        int x, y;

		vec2(int _x = 0.f, int _y = 0.f) : x(_x), y(_y) { }
        explicit vec2(const int* v) : x(v[0]), y(v[1]) { }

        auto empty() const -> bool {
            return x == 0.f && y == 0.f;
        }
        auto clear() -> void {
            x = 0.f;
            y = 0.f;
        }

        auto operator==(const vec2& other) const -> bool {
            return (x == other.x && y == other.y);
        }
        auto operator!=(const vec2& other) const -> bool {
            return !(*this == other);
        }

        template <typename A>
        auto operator+(A other) const -> vec2 {
            auto buf = *this;
            buf += static_cast<int>(other);
            return buf;
        }

        template <typename A>
        auto operator-(A other) const -> vec2 {
            auto buf = *this;
            buf -= static_cast<int>(other);
            return buf;
        }

        template <typename A>
        auto operator*(A other) const -> vec2 {
            auto buf = *this;
            buf *= static_cast<int>(other);
            return buf;
        }

        template <typename A>
        auto operator/(A other) const -> vec2 {
            auto buf = *this;
            buf /= static_cast<int>(other);
            return buf;
        }

        template <typename A>
        auto operator+=(A other) -> vec2& {
            x += static_cast<int>(other);
            y += static_cast<int>(other);

            return *this;
        }

        template <typename A>
        auto operator-=(A other) -> vec2& {
            x -= static_cast<int>(other);
            y -= static_cast<int>(other);

            return *this;
        }

        template <typename A>
        auto operator*=(A other) -> vec2& {
            x *= static_cast<int>(other);
            y *= static_cast<int>(other);

            return *this;
        }

        template <typename A>
        auto operator/=(A other) -> vec2& {
            x /= static_cast<int>(other);
            y /= static_cast<int>(other);

            return *this;
        }

        auto operator+(const vec2& other) const -> vec2 {
            auto buf = *this;

            buf.x += other.x;
            buf.y += other.y;

            return buf;
        }
        auto operator-(const vec2& other) const -> vec2 {
            auto buf = *this;

            buf.x -= other.x;
            buf.y -= other.y;

            return buf;
        }
        auto operator+=(const vec2& other) -> vec2& {
            x += other.x;
            y += other.y;

            return (*this);
        }
        auto operator-=(const vec2& other) -> vec2& {
            x -= other.x;
            y -= other.y;

            return (*this);
        }

        auto length() const -> int {
            return sqrt(length_sqr());
        }
        auto length_sqr() const -> int {
            return x * x + y * y;
        }
        auto distance(const vec2& other) const -> int {
            return (other - (*this)).length();
        }
        auto normalise() -> void {
            *this /= length();
        }
        auto normalised() const -> vec2 {
            auto vec = *this;
            vec.normalise();
            return vec;
        }
    };

    class vec3 {
    public:
        float x, y, z;

		inline auto init(float ix, float iy, float iz) -> void {
			x = ix;
			y = iy;
			z = iz;
		}

        vec3(float _x = 0.f, float _y = 0.f, float _z = 0.f) : x(_x), y(_y), z(_z) { }
        explicit vec3(const float* v) : x(v[0]), y(v[1]), z(v[2]) { }

        auto empty() const -> bool {
            return x == 0.f && y == 0.f && z == 0.f;
        }
        auto clear() -> void {
            x = 0.f;
            y = 0.f;
            z = 0.f;
        }

        auto operator=(const vec2& other) -> vec3& {
            x = other.x;
            y = other.y;
            z = 0.f;
            return *this;
        }
        auto operator==(const vec3& other) const -> bool {
            return (x == other.x && y == other.y && z == other.z);
        }
        auto operator!=(const vec3& other) const -> bool {
            return !(*this == other);
        }

        template <typename A>
        auto operator+(A other) const -> vec3 {
            auto buf = *this;
            buf += static_cast<float>(other);
            return buf;
        }

        template <typename A>
        auto operator-(A other) const -> vec3 {
            auto buf = *this;
            buf -= static_cast<float>(other);
            return buf;
        }

        template <typename A>
        auto operator*(A other) const -> vec3 {
            auto buf = *this;
            buf *= static_cast<float>(other);
            return buf;
        }

        template <typename A>
        auto operator/(A other) const -> vec3 {
            auto buf = *this;
            buf /= static_cast<float>(other);
            return buf;
        }

        template <typename A>
        auto operator+=(A other) -> vec3& {
            x += static_cast<float>(other);
            y += static_cast<float>(other);
            z += static_cast<float>(other);

            return *this;
        }

        template <typename A>
        auto operator-=(A other) -> vec3& {
            x -= static_cast<float>(other);
            y -= static_cast<float>(other);
            z -= static_cast<float>(other);

            return *this;
        }

        template <typename A>
        auto operator*=(A other) -> vec3& {
            x *= static_cast<float>(other);
            y *= static_cast<float>(other);
            z *= static_cast<float>(other);

            return *this;
        }

        template <typename A>
        auto operator/=(A other) -> vec3& {
            x /= static_cast<float>(other);
            y /= static_cast<float>(other);
            z /= static_cast<float>(other);

            return *this;
        }

        auto operator+(const vec3& other) const -> vec3 {
            auto buf = *this;

            buf.x += other.x;
            buf.y += other.y;
            buf.z += other.z;

            return buf;
        }
        auto operator-(const vec3& other) const -> vec3 {
            auto buf = *this;

            buf.x -= other.x;
            buf.y -= other.y;
            buf.z -= other.z;

            return buf;
        }
        auto operator+=(const vec3& other) -> vec3& {
            x += other.x;
            y += other.y;
            z += other.z;

            return (*this);
        }
        auto operator-=(const vec3& other) -> vec3& {
            x -= other.x;
            y -= other.y;
            z -= other.z;

            return (*this);
        }

        auto length_2d() const -> float {
            return sqrt(length_sqr_2d());
        }
        auto length_sqr_2d() const -> float {
            return x * x + y * y;
        }
        auto length() const -> float {
            return sqrt(length_sqr());
        }
        auto length_sqr() const -> float {
            return x * x + y * y + z * z;
        }
        auto distance(const vec3& other) const -> float {
            return abs((other - *this).length());
        }
		bool is_zero( )	{
			valid_yes( *this );
			if ( this->x == 0.f && this->y == 0.f && this->z == 0.f )
				return true;

			return false;
		}
		auto normalize_in_place() -> float {
			vec3& v = *this;

			float iradius = 1.f / (this->length_sqr() + 1.192092896e-07F); //FLT_EPSILON

			v.x *= iradius;
			v.y *= iradius;
			v.z *= iradius;
			return iradius;
		}

        auto normalise() -> void {
            *this /= length() + std::numeric_limits<float>::epsilon();
        }
        auto normalised() const -> vec3 {
            auto vec = *this;
            vec.normalise();
            return vec;
        }
        auto dot(const vec3& other) const -> float {
            return x * other.x + y * other.y + z * other.z;
        }
        auto dot(float* other) const -> float {
            return x * other[0] + y * other[1] + z * other[2];
        }
    };

    class vec4 {
    public:
        float x, y, z, w;

        vec4(float _x = 0.f, float _y = 0.f, float _z = 0.f, float _w = 0.f) : x(_x), y(_y), z(_z), w(_w) { }
        explicit vec4(const float* v) : x(v[0]), y(v[1]), z(v[2]), w(v[3]) { }

        auto empty() const -> bool {
            return x == 0.f && y == 0.f && z == 0.f && w == 0.f;
        }
        auto clear() -> void {
            x = 0.f;
            y = 0.f;
            z = 0.f;
            w = 0.f;
        }

        auto operator=(const vec2& other) -> vec4& {
            x = other.x;
            y = other.y;
            z = 0.f;
            w = 0.f;
            return *this;
        }
        auto operator=(const vec3& other) -> vec4& {
            x = other.x;
            y = other.y;
            z = other.z;
            w = 0.f;
            return *this;
        }

        auto operator==(const vec4& other) const -> bool {
            return (x == other.x && y == other.y && z == other.z && w == other.w);
        }
        auto operator!=(const vec4& other) const -> bool {
            return !(*this == other);
        }

        template <typename A>
        auto operator+(A other) const -> vec4 {
            auto buf = *this;
            buf += static_cast<float>(other);
            return buf;
        }

        template <typename A>
        auto operator-(A other) const -> vec4 {
            auto buf = *this;
            buf -= static_cast<float>(other);
            return buf;
        }

        template <typename A>
        auto operator*(A other) const -> vec4 {
            auto buf = *this;
            buf *= static_cast<float>(other);
            return buf;
        }

        template <typename A>
        auto operator/(A other) const -> vec4 {
            auto buf = *this;
            buf /= static_cast<float>(other);
            return buf;
        }

        template <typename A>
        auto operator+=(A other) -> vec4& {
            x += static_cast<float>(other);
            y += static_cast<float>(other);
            z += static_cast<float>(other);
            w += static_cast<float>(other);

            return *this;
        }

        template <typename A>
        auto operator-=(A other) -> vec4& {
            x -= static_cast<float>(other);
            y -= static_cast<float>(other);
            z -= static_cast<float>(other);
            w -= static_cast<float>(other);

            return *this;
        }

        template <typename A>
        auto operator*=(A other) -> vec4& {
            x *= static_cast<float>(other);
            y *= static_cast<float>(other);
            z *= static_cast<float>(other);
            w *= static_cast<float>(other);

            return *this;
        }

        template <typename A>
        auto operator/=(A other) -> vec4& {
            x /= static_cast<float>(other);
            y /= static_cast<float>(other);
            z /= static_cast<float>(other);
            w /= static_cast<float>(other);

            return *this;
        }

        auto operator+(const vec4& other) const -> vec4 {
            auto buf = *this;

            buf.x += other.x;
            buf.y += other.y;
            buf.z += other.z;
            buf.w += other.w;

            return buf;
        }
        auto operator-(const vec4& other) const -> vec4 {
            auto buf = *this;

            buf.x -= other.x;
            buf.y -= other.y;
            buf.z -= other.z;
            buf.w -= other.w;

            return buf;
        }
        auto operator+=(const vec4& other) -> vec4& {
            x += other.x;
            y += other.y;
            z += other.z;
            w += other.w;

            return (*this);
        }
        auto operator-=(const vec4& other) -> vec4& {
            x -= other.x;
            y -= other.y;
            z -= other.z;
            w -= other.w;

            return (*this);
        }

        auto length_2d() const -> float {
            return std::sqrt(length_sqr_2d());
        }
        auto length_sqr_2d() const -> float {
            return x * x + y * y;
        }
        auto length_3d() const -> float {
            return std::sqrt(length_sqr_3d());
        }
        auto length_sqr_3d() const -> float {
            return x * x + y * y + z * z;
        }
        auto length() const -> float {
            return std::sqrt(length_sqr());
        }
        auto length_sqr() const -> float {
            return x * x + y * y + z * z + w * w;
        }
        auto distance(const vec4& other) const -> float {
            return (other - (*this)).length();
        }
        auto normalise() -> void {
            *this /= length();
        }
        auto normalised() const -> vec4 {
            auto vec = *this;
            vec.normalise();
            return vec;
        }
    };

	class quaternion
	{
	public:
		float x, y, z, w;

		float operator[]( int i ) const
		{
			if ( i == 1 )
				return x;
			if ( i == 2 )
				return y;
			if ( y == 3 )
				return z;
			return w;
		};

		float& operator[]( int i )
		{
			if ( i == 1 )
				return x;
			if ( i == 2 )
				return y;
			if ( y == 3 )
				return z;
			return w;
		};
	};

	template <class T, class I = int> class utl_memory
	{
	public:
		inline bool IsIdxValid( I i ) const
		{
			long x = i;
			return ( x >= 0 ) && ( x < m_nAllocationCount );
		}
		T& operator[]( I i );
		const T& operator[]( I i ) const;
		T *Base( )
		{
			return m_pMemory;
		}
		inline int NumAllocated( ) const
		{
			return m_nAllocationCount;
		}
		void Grow( int num )
		{
			assert( num > 0 );

			auto oldAllocationCount = m_nAllocationCount;
			// Make sure we have at least numallocated + num allocations.
			// Use the grow rules specified for this memory (in m_nGrowSize)
			int nAllocationRequested = m_nAllocationCount + num;

			int nNewAllocationCount = UtlMemory_CalcNewAllocationCount( m_nAllocationCount, m_nGrowSize, nAllocationRequested, sizeof( T ) );

			// if m_nAllocationRequested wraps index type I, recalculate
			if ( ( int )( I )nNewAllocationCount < nAllocationRequested )
			{
				if ( ( int )( I )nNewAllocationCount == 0 && ( int )( I )( nNewAllocationCount - 1 ) >= nAllocationRequested )
				{
					--nNewAllocationCount; // deal w/ the common case of m_nAllocationCount == MAX_USHORT + 1
				}
				else
				{
					if ( ( int )( I )nAllocationRequested != nAllocationRequested )
					{
						// we've been asked to grow memory to a size s.t. the index type can't address the requested amount of memory
						assert( 0 );
						return;
					}
					while ( ( int )( I )nNewAllocationCount < nAllocationRequested )
					{
						nNewAllocationCount = ( nNewAllocationCount + nAllocationRequested ) / 2;
					}
				}
			}

			m_nAllocationCount = nNewAllocationCount;

			if ( m_pMemory )
			{
				auto ptr = new unsigned char[ m_nAllocationCount * sizeof( T ) ];

				memcpy( ptr, m_pMemory, oldAllocationCount * sizeof( T ) );
				m_pMemory = ( T* )ptr;
			}
			else
			{
				m_pMemory = ( T* )new unsigned char[ m_nAllocationCount * sizeof( T ) ];
			}
		}
	protected:
		T * m_pMemory;
		int m_nAllocationCount;
		int m_nGrowSize;
	};

	template< class T, class I >
	inline T& utl_memory<T, I>::operator[]( I i )
	{
		assert( IsIdxValid( i ) );
		return m_pMemory[ i ];
	}

	template< class T, class I >
	inline const T& utl_memory<T, I>::operator[]( I i ) const
	{
		assert( IsIdxValid( i ) );
		return m_pMemory[ i ];
	}

	template <class T>
	inline void Destruct( T *pMemory )
	{
		pMemory->~T( );
	}

	template <class T>
	inline T* Construct( T* pMemory )
	{
		return ::new( pMemory ) T;
	}

	template< class T, class A = utl_memory<T> >
	class utl_vector
	{
		typedef A CAllocator;

		typedef T *iterator;
		typedef const T *const_iterator;
	public:
		T & operator[]( int i );
		const T& operator[]( int i ) const;

		T& Element( int i )
		{
			return m_Memory[ i ];
		}

		T* Base( )
		{
			return m_Memory.Base( );
		}

		int count( ) const
		{
			return m_Size;
		}

		void remove_all( )
		{
			for ( int i = m_Size; --i >= 0; )
				Destruct( &Element( i ) );

			m_Size = 0;
		}

		inline bool IsValidIndex( int i ) const
		{
			return ( i >= 0 ) && ( i < m_Size );
		}

		void GrowVector( int num = 1 )
		{
			if ( m_Size + num > m_Memory.NumAllocated( ) )
			{
				m_Memory.Grow( m_Size + num - m_Memory.NumAllocated( ) );
			}

			m_Size += num;
		}

		int InsertBefore( int elem )
		{
			// Can insert at the end
			assert( ( elem == count( ) ) || IsValidIndex( elem ) );

			GrowVector( );
			Construct( &Element( elem ) );
			return elem;
		}

		inline int AddToHead( )
		{
			return InsertBefore( 0 );
		}

		inline int AddToTail( )
		{
			return InsertBefore( m_Size );
		}

		iterator begin( ) { return Base( ); }
		const_iterator begin( ) const { return Base( ); }
		iterator end( ) { return Base( ) + count( ); }
		const_iterator end( ) const { return Base( ) + count( ); }

	protected:
		CAllocator m_Memory;
		int m_Size;
		T *m_pElements;
	};

	template< typename T, class A >
	inline T& utl_vector<T, A>::operator[]( int i )
	{
		assert( i < m_Size );
		return m_Memory[ i ];
	}

	template< typename T, class A >
	inline const T& utl_vector<T, A>::operator[]( int i ) const
	{
		assert( i < m_Size );
		return m_Memory[ i ];
	}
}