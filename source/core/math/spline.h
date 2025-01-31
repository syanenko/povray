//******************************************************************************
///
/// @file core/math/spline.h
///
/// Declarations for spline-related maths.
///
/// @note   This file currently contains only stuff for the SDL's function-alike
///         spline feature; as such, it would naturally belong in the parser
///         module. However, it is planned for the polymorphic type hierarchy
///         herein to also absorb the spline-specific maths for the geometric
///         primtitives (which is currently embedded in the respective
///         primitives' code), and the file has already been moved to the core
///         module in preparation.
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.8.
/// Copyright 1991-2019 Persistence of Vision Raytracer Pty. Ltd.
///
/// POV-Ray is free software: you can redistribute it and/or modify
/// it under the terms of the GNU Affero General Public License as
/// published by the Free Software Foundation, either version 3 of the
/// License, or (at your option) any later version.
///
/// POV-Ray is distributed in the hope that it will be useful,
/// but WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
/// GNU Affero General Public License for more details.
///
/// You should have received a copy of the GNU Affero General Public License
/// along with this program.  If not, see <http://www.gnu.org/licenses/>.
///
/// ----------------------------------------------------------------------------
///
/// POV-Ray is based on the popular DKB raytracer version 2.12.
/// DKBTrace was originally written by David K. Buck.
/// DKBTrace Ver 2.0-2.12 were written by David K. Buck & Aaron A. Collins.
///
/// @endparblock
///
//******************************************************************************

#ifndef POVRAY_CORE_SPLINE_H
#define POVRAY_CORE_SPLINE_H

// Module config header file must be the first file included within POV-Ray unit header files
#include "core/configcore.h"

// C++ variants of C standard header files
//  (none at the moment)

// C++ standard header files
#include <vector>

// POV-Ray header files (base module)
#include "base/base_fwd.h"

// POV-Ray header files (core module)
//  (none at the moment)

namespace pov
{

using namespace pov_base;

//##############################################################################
///
/// @defgroup PovCoreMathSpline Splines
/// @ingroup PovCoreMath
///
/// @{

struct SplineEntry final
{
    DBL par;      // Parameter
    EXPRESS vec;  // Value at the parameter
};
struct SplineCoeff
{
    DBL coeff[5]; // Interpolating coefficients at the parameter
};

struct SplineCoeffFour
{
    DBL coeff[5][4]; // Interpolating coefficients at the parameter
};

struct SplineTcbParam
{
  DBL tension;
  DBL bias;
  DBL continuity;
  SplineTcbParam():tension(0.0),bias(0.0), continuity(0.0){}
};

struct SplineFreedom
{
  DBL freedom_degree;
  SplineFreedom():freedom_degree(0.0){}
};

typedef std::vector<SplineEntry> SplineEntryList;
typedef std::vector<SplineCoeff> SplineCoeffList;
typedef std::vector<SplineCoeffFour> SplineCoeffFourList;
typedef std::vector<SplineTcbParam> SplineTcbParamList;
typedef std::vector<SplineFreedom> SplineFreedomList;

typedef int SplineRefCount;

struct GenericSpline
{
    GenericSpline();
    GenericSpline(const GenericSpline& o);
    virtual ~GenericSpline();
    SplineEntryList SplineEntries;
    bool Coeffs_Computed;
    int Terms;
    SplineRefCount ref_count;

    virtual void Get(DBL p, EXPRESS& v) = 0;
    virtual GenericSpline* Clone() const = 0;
    void AcquireReference();
    void ReleaseReference();
    // indicate to the parser which additional parameters to collect
    enum class Extension
    {
      None,
      TCB,
      GlobalFreedom,
      Freedom
    };
    virtual Extension Extended()const{ return Extension::None;}
};

struct LinearSpline final : public GenericSpline
{
    LinearSpline();
    LinearSpline(const GenericSpline& o);
    virtual void Get(DBL p, EXPRESS& v) override;
    virtual GenericSpline* Clone() const override { return new LinearSpline(*this); }
};

struct QuadraticSpline final : public GenericSpline
{
    QuadraticSpline();
    QuadraticSpline(const GenericSpline& o);
    virtual void Get(DBL p, EXPRESS& v) override;
    virtual GenericSpline* Clone() const override { return new QuadraticSpline(*this); }
};

struct NaturalSpline final : public GenericSpline
{
    NaturalSpline();
    NaturalSpline(const GenericSpline& o);
    virtual void Get(DBL p, EXPRESS& v) override;
    virtual GenericSpline* Clone() const override { return new NaturalSpline(*this); }
private:
    SplineCoeffList SplinePreComputed;
    void Precompute();
};

struct CatmullRomSpline final : public GenericSpline
{
    CatmullRomSpline();
    CatmullRomSpline(const GenericSpline& o);
    virtual void Get(DBL p, EXPRESS& v) override;
    virtual GenericSpline* Clone() const override { return new CatmullRomSpline(*this); }
};

struct SorSpline: public GenericSpline
{
    SorSpline();
    SorSpline(const GenericSpline& o);
    virtual void Get(DBL p, EXPRESS& v) override;
    virtual GenericSpline* Clone() const override { return new SorSpline(*this); }
private:
    SplineCoeffFourList SplinePreComputed;
    void Precompute();
    DBL interpolate(int i, int k, DBL p)const;
};

struct AkimaSpline: public GenericSpline
{
    AkimaSpline();
    AkimaSpline(const GenericSpline& o);
    virtual void Get(DBL p, EXPRESS& v) override;
    virtual GenericSpline* Clone() const override { return new AkimaSpline(*this); }
private:
    SplineCoeffFourList SplinePreComputed;
    void Precompute();
    DBL interpolate(int i, int k, DBL p)const;
};

struct TcbSpline: public GenericSpline
{
    TcbSpline();
    TcbSpline(const GenericSpline& o);
    virtual void Get(DBL p, EXPRESS& v) override;
    virtual GenericSpline* Clone() const override { return new TcbSpline(*this); }
    SplineTcbParamList in,out;
    virtual Extension Extended()const override { return Extension::TCB;}
private:
    SplineCoeffList SplinePreComputedIn;
    SplineCoeffList SplinePreComputedOut;
    void Precompute();
    DBL interpolate(int i, int k, DBL p)const;
};

/* abstract common class for ExtendedXSpline and GeneralXSpline */
struct XSpline: public GenericSpline
{
    XSpline();
    XSpline( const GenericSpline& o );
    virtual void Get(DBL p, EXPRESS& v) override;
    virtual GenericSpline* Clone() const override =0;
    SplineFreedomList node;
    virtual Extension Extended()const override { return Extension::Freedom;}
protected:
    virtual DBL interpolate(int i, int k, DBL p, int N)const=0;
};

struct BasicXSpline: public GenericSpline
{
    BasicXSpline();
    BasicXSpline( const GenericSpline& o );
    virtual void Get(DBL p, EXPRESS& v) override;
    virtual GenericSpline* Clone() const override { return new BasicXSpline(*this); }
    SplineFreedom freedom;
    virtual Extension Extended()const override { return Extension::GlobalFreedom;}
private:
    DBL interpolate( int i, int k, DBL p, DBL fd)const;

};

struct ExtendedXSpline: public XSpline
{
    ExtendedXSpline();
    ExtendedXSpline( const GenericSpline& o );
    virtual GenericSpline* Clone() const override { return new ExtendedXSpline(*this); }
protected:
    virtual DBL interpolate(int i, int k, DBL p, int N)const override;
};


struct GeneralXSpline: public XSpline
{
    GeneralXSpline();
    GeneralXSpline( const GenericSpline& o );
    virtual GenericSpline* Clone() const override { return new GeneralXSpline(*this); }
protected:
    virtual DBL interpolate(int i, int k, DBL p, int N)const override;

};
// TODO FIXME - Some of the following are higher-level functions and should be moved to the parser, others should be made part of the class.

GenericSpline* Copy_Spline(const GenericSpline* Old);
void Acquire_Spline_Reference(GenericSpline* sp);
void Release_Spline_Reference(GenericSpline* sp);
void Destroy_Spline(GenericSpline* sp);
void Insert_Spline_Entry(GenericSpline* sp, DBL p, const EXPRESS& v);
void Insert_Spline_Entry(GenericSpline* sp, DBL p, const EXPRESS& v, const SplineTcbParam& in, const SplineTcbParam& out);
void Insert_Spline_Entry(GenericSpline* sp, DBL p, const EXPRESS& v, const SplineFreedom& freedom );
DBL Get_Spline_Val(GenericSpline* sp, DBL p, EXPRESS& v, int *Terms);

/// @}
///
//##############################################################################

}
// end of namespace pov

#endif // POVRAY_CORE_SPLINE_H
