#pragma once

#include "Definitions.hpp"
#include "Parameters.hpp"

namespace Stencils {

  // Load the local velocity cube with relevant velocities of the 2D plane
  inline void loadLocalVelocity2D(FlowField& flowField, RealType* const localVelocity, int i, int j) {
    for (int row = -1; row <= 1; row++) {
      for (int column = -1; column <= 1; column++) {
        const RealType* const point                  = flowField.getVelocity().getVector(i + column, j + row);
        localVelocity[39 + 9 * row + 3 * column]     = point[0]; // x-component
        localVelocity[39 + 9 * row + 3 * column + 1] = point[1]; // y-component
      }
    }
  }

  // Load the local velocity cube with surrounding velocities
  inline void loadLocalVelocity3D(FlowField& flowField, RealType* const localVelocity, int i, int j, int k) {
    for (int layer = -1; layer <= 1; layer++) {
      for (int row = -1; row <= 1; row++) {
        for (int column = -1; column <= 1; column++) {
          const RealType* const point = flowField.getVelocity().getVector(i + column, j + row, k + layer);
          localVelocity[39 + 27 * layer + 9 * row + 3 * column]     = point[0]; // x-component
          localVelocity[39 + 27 * layer + 9 * row + 3 * column + 1] = point[1]; // y-component
          localVelocity[39 + 27 * layer + 9 * row + 3 * column + 2] = point[2]; // z-component
        }
      }
    }
  }

  // Load local meshsize for 2D -> same as loadLocalVelocity2D, but invoking call to meshsize-ptr
  inline void loadLocalMeshsize2D(const Parameters& parameters, RealType* const localMeshsize, int i, int j) {
    for (int row = -1; row <= 1; row++) {
      for (int column = -1; column <= 1; column++) {
        localMeshsize[39 + 9 * row + 3 * column]     = parameters.meshsize->getDx(i + column, j + row);
        localMeshsize[39 + 9 * row + 3 * column + 1] = parameters.meshsize->getDy(i + column, j + row);
      }
    }
  }

  // Load local meshsize for 3D
  inline void loadLocalMeshsize3D(const Parameters& parameters, RealType* const localMeshsize, int i, int j, int k) {
    for (int layer = -1; layer <= 1; layer++) {
      for (int row = -1; row <= 1; row++) {
        for (int column = -1; column <= 1; column++) {
          localMeshsize[39 + 27 * layer + 9 * row + 3 * column] = parameters.meshsize->getDx(
            i + column, j + row, k + layer
          );
          localMeshsize[39 + 27 * layer + 9 * row + 3 * column + 1] = parameters.meshsize->getDy(
            i + column, j + row, k + layer
          );
          localMeshsize[39 + 27 * layer + 9 * row + 3 * column + 2] = parameters.meshsize->getDz(
            i + column, j + row, k + layer
          );
        }
      }
    }
  }

  // Maps an index and a component to the corresponding value in the cube.
  inline int mapd(int i, int j, int k, int component) { return 39 + 27 * k + 9 * j + 3 * i + component; }

  // Derivative functions. They are applied to a cube of 3x3x3 cells. lv stands for the local velocity, lm represents
  // the local mesh sizes dudx <-> first derivative of u-component of velocity field w.r.t. x-direction.
  inline RealType dudx(const RealType* const lv, const RealType* const lm) {
    // Evaluate dudx in the cell center by a central difference
    const int index0 = mapd(0, 0, 0, 0);
    const int index1 = mapd(-1, 0, 0, 0);
    return (lv[index0] - lv[index1]) / lm[index0];
  }

  inline RealType dvdy(const RealType* const lv, const RealType* const lm) {
    const int index0 = mapd(0, 0, 0, 1);
    const int index1 = mapd(0, -1, 0, 1);
    return (lv[index0] - lv[index1]) / lm[index0];
  }

  inline RealType dwdz(const RealType* const lv, const RealType* const lm) {
    const int index0 = mapd(0, 0, 0, 2);
    const int index1 = mapd(0, 0, -1, 2);
    return (lv[index0] - lv[index1]) / lm[index0];
  }

    // TODO WS1: Second derivatives

  // Second derivatives of u, v, and w components
  inline RealType d2udx2(const RealType* const lv, const RealType* const lm) {
    // Evaluate d2udx2 in the cell center by a central difference
    const int index_u0 = mapd(0, 0, 0, 0); // u at center
    const int index_u1 = mapd(-1, 0, 0, 0); // u at left
    const int index_u2 = mapd(1, 0, 0, 0);  // u at right

    const int index_x0 = mapd(0, 0, 0, 0); // x at center
    const int index_x1 = mapd(-1, 0, 0, 0); // x at left
    const int index_x2 = mapd(1, 0, 0, 0);  // x at right
    return 2*((lv[index_u2])/(lm[index_x2] * (lm[index_x2] + lm[index_x0]))
    -(lv[index_u0])/(lm[index_x0] * lm[index_x2])
    +(lv[index_u1])/(lm[index_x0]*(lm[index_x0]+lm[index_x2])));
    //return (lv[index2] - 2 * lv[index0] + lv[index1]) / (lm[index00] * lm[index00]);
  }

  inline RealType d2vdx2(const RealType* const lv, const RealType* const lm) {
    // Evaluate d2vdx2 in the cell center by a central difference
    const int index_u0 = mapd(0, 0, 0, 1); // v at center
    const int index_u1 = mapd(-1, 0, 0, 1); // v at left
    const int index_u2 = mapd(1, 0, 0, 1);  // v at right

    const int index_x0 = mapd(0, 0, 0, 0); // x at center
    const int index_x1 = mapd(-1, 0, 0, 0); // x at left
    const int index_x2 = mapd(1, 0, 0, 0);  // x at right
    return 2*((lv[index_u2])/(lm[index_x2] * (lm[index_x2] + lm[index_x0]))-(lv[index_u0])/(lm[index_x0] * lm[index_x2])+(lv[index_u1])/(lm[index_x0]*(lm[index_x0]+lm[index_x2])));
    //return (lv[index2] - 2 * lv[index0] + lv[index1]) / (lm[index00] * lm[index00]);
  }

  inline RealType d2wdx2(const RealType* const lv, const RealType* const lm) {
    // Evaluate d2wdx2 in the cell center by a central difference
    const int index_u0 = mapd(0, 0, 0, 2); // w at center
    const int index_u1 = mapd(-1, 0, 0, 2); // w at left
    const int index_u2 = mapd(1, 0, 0, 2);  // w at right

    const int index_x0 = mapd(0, 0, 0, 0); // x at center
    const int index_x1 = mapd(-1, 0, 0, 0); // x at left
    const int index_x2 = mapd(1, 0, 0, 0);  // x at right
    return 2*((lv[index_u2])/(lm[index_x2] * (lm[index_x2] + lm[index_x0]))-(lv[index_u0])/(lm[index_x0] * lm[index_x2])+(lv[index_u1])/(lm[index_x0]*(lm[index_x0]+lm[index_x2])));
    //return (lv[index2] - 2 * lv[index0] + lv[index1]) / (lm[index00] * lm[index00]);
  }

  // Similarly for y directions
  inline RealType d2udy2(const RealType* const lv, const RealType* const lm) {
    const int index_u0 = mapd(0, 0, 0, 0); // u at center
    const int index_u1 = mapd(0, -1, 0, 0); // u at left
    const int index_u2 = mapd(0, 1, 0, 0);  // u at right

    const int index_x0 = mapd(0, 0, 0, 1); // y at center
    const int index_x1 = mapd(0, -1, 0, 1); // y at left
    const int index_x2 = mapd(0, 1, 0, 1);  // y at right
    return 2*((lv[index_u2])/(lm[index_x2] * (lm[index_x2] + lm[index_x0]))-(lv[index_u0])/(lm[index_x0] * lm[index_x2])+(lv[index_u1])/(lm[index_x0]*(lm[index_x0]+lm[index_x2])));
    //return (lv[index2] - 2 * lv[index0] + lv[index1]) / (lm[index0] * lm[index0]);
  }

  inline RealType d2vdy2(const RealType* const lv, const RealType* const lm) {
    const int index_u0 = mapd(0, 0, 0, 1); // v at center
    const int index_u1 = mapd(0, -1, 0, 1); // v at left
    const int index_u2 = mapd(0, 1, 0, 1);  // v at right

    const int index_x0 = mapd(0, 0, 0, 1); // y at center
    const int index_x1 = mapd(0, -1, 0, 1); // y at left
    const int index_x2 = mapd(0, 1, 0, 1);  // y at right
    return 2*((lv[index_u2])/(lm[index_x2] * (lm[index_x2] + lm[index_x0]))-(lv[index_u0])/(lm[index_x0] * lm[index_x2])+(lv[index_u1])/(lm[index_x0]*(lm[index_x0]+lm[index_x2])));
    //return (lv[index2] - 2 * lv[index0] + lv[index1]) / (lm[index0] * lm[index0]);
  }

  inline RealType d2wdy2(const RealType* const lv, const RealType* const lm) {
    const int index_u0 = mapd(0, 0, 0, 2); // w at center
    const int index_u1 = mapd(0, -1, 0, 2); // w at left
    const int index_u2 = mapd(0, 1, 0, 2);  // w at right

    const int index_x0 = mapd(0, 0, 0, 1); // y at center
    const int index_x1 = mapd(0, -1, 0, 1); // y at left
    const int index_x2 = mapd(0, 1, 0, 1);  // y at right
    return 2*((lv[index_u2])/(lm[index_x2] * (lm[index_x2] + lm[index_x0]))-(lv[index_u0])/(lm[index_x0] * lm[index_x2])+(lv[index_u1])/(lm[index_x0]*(lm[index_x0]+lm[index_x2])));
    //return (lv[index2] - 2 * lv[index0] + lv[index1]) / (lm[index0] * lm[index0]);
  }

  // And for z direction
  inline RealType d2udz2(const RealType* const lv, const RealType* const lm) {
    const int index_u0 = mapd(0, 0, 0, 0); // u at center
    const int index_u1 = mapd(0, 0, -1, 0); // u at left
    const int index_u2 = mapd(0, 0, 1, 0);  // u at right

    const int index_x0 = mapd(0, 0, 0, 0); // z at center
    const int index_x1 = mapd(0, 0, -1, 0); // z at left
    const int index_x2 = mapd(0, 0, 1, 0);  // z at right
    return 2*((lv[index_u2])/(lm[index_x2] * (lm[index_x2] + lm[index_x0]))-(lv[index_u0])/(lm[index_x0] * lm[index_x2])+(lv[index_u1])/(lm[index_x0]*(lm[index_x0]+lm[index_x2])));
    //return (lv[index2] - 2 * lv[index0] + lv[index1]) / (lm[index0] * lm[index0]);
  }

  inline RealType d2vdz2(const RealType* const lv, const RealType* const lm) {
    const int index_u0 = mapd(0, 0, 0, 1); // v at center
    const int index_u1 = mapd(0, 0, -1, 1); // v at left
    const int index_u2 = mapd(0, 0, 1, 1);  // v at right

    const int index_x0 = mapd(0, 0, 0, 0); // z at center
    const int index_x1 = mapd(0, 0, -1, 0); // z at left
    const int index_x2 = mapd(0, 0, 1, 0);  // z at right
    return 2*((lv[index_u2])/(lm[index_x2] * (lm[index_x2] + lm[index_x0]))-(lv[index_u0])/(lm[index_x0] * lm[index_x2])+(lv[index_u1])/(lm[index_x0]*(lm[index_x0]+lm[index_x2])));
    //return (lv[index2] - 2 * lv[index0] + lv[index1]) / (lm[index0] * lm[index0]);
  }

  // Similarly for w, you would do:
  inline RealType d2wdz2(const RealType* const lv, const RealType* const lm) {
    const int index_u0 = mapd(0, 0, 0, 2); // w at center
    const int index_u1 = mapd(0, 0, -1, 2); // w at left
    const int index_u2 = mapd(0, 0, 1, 2);  // w at right

    const int index_x0 = mapd(0, 0, 0, 0); // z at center
    const int index_x1 = mapd(0, 0, -1, 0); // z at left
    const int index_x2 = mapd(0, 0, 1, 0);  // z at right
    return 2*((lv[index_u2])/(lm[index_x2] * (lm[index_x2] + lm[index_x0]))-(lv[index_u0])/(lm[index_x0] * lm[index_x2])+(lv[index_u1])/(lm[index_x0]*(lm[index_x0]+lm[index_x2])));
    //return (lv[index2] - 2 * lv[index0] + lv[index1]) / (lm[index0] * lm[index0]);
  }


  // First derivative of product (u*v), evaluated at the location of the v-component.
  inline RealType duvdx(const RealType* const lv, const Parameters& parameters, const RealType* const lm) {
#ifndef NDEBUG
    const RealType tmp1 = 1.0 / 4.0 * ((((lv[mapd(0, 0, 0, 0)] + lv[mapd(0, 1, 0, 0)]) *
        (lv[mapd(0, 0, 0, 1)] + lv[mapd(1, 0, 0, 1)])) -
        ((lv[mapd(-1, 0, 0, 0)] + lv[mapd(-1, 1, 0, 0)]) *
            (lv[mapd(-1, 0, 0, 1)] + lv[mapd(0, 0, 0, 1)])))
        + parameters.solver.gamma * ((fabs(lv[mapd(0, 0, 0, 0)] + lv[mapd(0, 1, 0, 0)]) *
            (lv[mapd(0, 0, 0, 1)] - lv[mapd(1, 0, 0, 1)])) -
            (fabs(lv[mapd(-1, 0, 0, 0)] + lv[mapd(-1, 1, 0, 0)]) *
                (lv[mapd(-1, 0, 0, 1)] - lv[mapd(0, 0, 0, 1)])))
        ) / lm[mapd(0, 0, 0, 0)];
#endif

    const RealType hxShort = 0.5 * lm[mapd(0, 0, 0, 0)]; // Distance of corner points in x-direction from center v-value
    const RealType hxLong0 = 0.5 * (lm[mapd(0, 0, 0, 0)] + lm[mapd(-1, 0, 0, 0)]); // Distance between center and west
                                                                                   // v-value
    const RealType hxLong1 = 0.5 * (lm[mapd(0, 0, 0, 0)] + lm[mapd(1, 0, 0, 0)]);  // Distance between center and east
                                                                                   // v-value
    const RealType hyShort = 0.5 * lm[mapd(0, 0, 0, 1)]; // Distance of center u-value from upper edge of cell
    const RealType hyLong = 0.5 * (lm[mapd(0, 0, 0, 1)] + lm[mapd(0, 1, 0, 1)]); // Distance of north and center u-value

    const RealType u00 = lv[mapd(0, 0, 0, 0)];
    const RealType u01 = lv[mapd(0, 1, 0, 0)];
    const RealType v00 = lv[mapd(0, 0, 0, 1)];
    const RealType v10 = lv[mapd(1, 0, 0, 1)];

    const RealType uM10 = lv[mapd(-1, 0, 0, 0)];
    const RealType uM11 = lv[mapd(-1, 1, 0, 0)];
    const RealType vM10 = lv[mapd(-1, 0, 0, 1)];

    // This a central difference expression for the first-derivative. We therefore linearly interpolate u*v onto the
    // surface of the current cell (in 2D: upper left and upper right corner) and then take the central difference.
    const RealType secondOrder = (((hyLong - hyShort) / hyLong * u00 + hyShort / hyLong * u01
                                  ) * ((hxLong1 - hxShort) / hxLong1 * v00 + hxShort / hxLong1 * v10)
                                  - ((hyLong - hyShort) / hyLong * uM10 + hyShort / hyLong * uM11
                                    ) * ((hxLong0 - hxShort) / hxLong0 * v00 + hxShort / hxLong0 * vM10))
                                 / (2.0 * hxShort);

    // This is a forward-difference in donor-cell style. We apply donor cell and again interpolate the velocity values
    // (u-comp.) onto the surface of the cell. We then apply the standard donor cell scheme. This will, however, result
    // in non-equal mesh spacing evaluations (in case of stretched meshes).
    const RealType kr = (hyLong - hyShort) / hyLong * u00 + hyShort / hyLong * u01;
    const RealType kl = (hyLong - hyShort) / hyLong * uM10 + hyShort / hyLong * uM11;

    const RealType firstOrder
      = 1.0 / (4.0 * hxShort)
        * (kr * (v00 + v10) - kl * (vM10 + v00) + fabs(kr) * (v00 - v10) - fabs(kl) * (vM10 - v00));

    // Return linear combination of central and donor-cell difference
    const RealType tmp2 = (1.0 - parameters.solver.gamma) * secondOrder + parameters.solver.gamma * firstOrder;

#ifndef NDEBUG
    if (fabs(tmp1 - tmp2) > 1.0e-12) {
      throw std::runtime_error("Error in duvdx");
    }
#endif

    return tmp2;
  }

  // Evaluates first derivative w.r.t. y for u*v at location of u-component. For details on implementation, see duvdx.
  inline RealType duvdy(const RealType* const lv, const Parameters& parameters, const RealType* const lm) {
#ifndef NDEBUG
    const RealType tmp1 = 1.0 / 4.0 * ((((lv[mapd(0, 0, 0, 1)] + lv[mapd(1, 0, 0, 1)]) *
        (lv[mapd(0, 0, 0, 0)] + lv[mapd(0, 1, 0, 0)])) -
        ((lv[mapd(0, -1, 0, 1)] + lv[mapd(1, -1, 0, 1)]) *
            (lv[mapd(0, -1, 0, 0)] + lv[mapd(0, 0, 0, 0)]))) +
        parameters.solver.gamma * ((fabs(lv[mapd(0, 0, 0, 1)] + lv[mapd(1, 0, 0, 1)]) *
            (lv[mapd(0, 0, 0, 0)] - lv[mapd(0, 1, 0, 0)])) -
            (fabs(lv[mapd(0, -1, 0, 1)] + lv[mapd(1, -1, 0, 1)]) *
                (lv[mapd(0, -1, 0, 0)] - lv[mapd(0, 0, 0, 0)])))) /
        lm[mapd(0, 0, 0, 1)];
#endif

    const RealType hyShort = 0.5 * lm[mapd(0, 0, 0, 1)]; // Distance of corner points in x-direction from center v-value
    const RealType hyLong0 = 0.5 * (lm[mapd(0, 0, 0, 1)] + lm[mapd(0, -1, 0, 1)]); // Distance between center and west
                                                                                   // v-value
    const RealType hyLong1 = 0.5 * (lm[mapd(0, 0, 0, 1)] + lm[mapd(0, 1, 0, 1)]);  // Distance between center and east
                                                                                   // v-value
    const RealType hxShort = 0.5 * lm[mapd(0, 0, 0, 0)]; // Distance of center u-value from upper edge of cell
    const RealType hxLong = 0.5 * (lm[mapd(0, 0, 0, 0)] + lm[mapd(1, 0, 0, 0)]); // Distance of north and center u-value

    const RealType v00 = lv[mapd(0, 0, 0, 1)];
    const RealType v10 = lv[mapd(1, 0, 0, 1)];
    const RealType u00 = lv[mapd(0, 0, 0, 0)];
    const RealType u01 = lv[mapd(0, 1, 0, 0)];

    const RealType v0M1 = lv[mapd(0, -1, 0, 1)];
    const RealType v1M1 = lv[mapd(1, -1, 0, 1)];
    const RealType u0M1 = lv[mapd(0, -1, 0, 0)];

    const RealType secondOrder = (((hxLong - hxShort) / hxLong * v00 + hxShort / hxLong * v10
                                  ) * ((hyLong1 - hyShort) / hyLong1 * u00 + hyShort / hyLong1 * u01)
                                  - ((hxLong - hxShort) / hxLong * v0M1 + hxShort / hxLong * v1M1
                                    ) * ((hyLong0 - hyShort) / hyLong0 * u00 + hyShort / hyLong0 * u0M1))
                                 / (2.0 * hyShort);

    const RealType kr = (hxLong - hxShort) / hxLong * v00 + hxShort / hxLong * v10;
    const RealType kl = (hxLong - hxShort) / hxLong * v0M1 + hxShort / hxLong * v1M1;

    const RealType firstOrder
      = 1.0 / (4.0 * hyShort)
        * (kr * (u00 + u01) - kl * (u0M1 + u00) + fabs(kr) * (u00 - u01) - fabs(kl) * (u0M1 - u00));

    const RealType tmp2 = (1.0 - parameters.solver.gamma) * secondOrder + parameters.solver.gamma * firstOrder;

#ifndef NDEBUG
    if (fabs(tmp1 - tmp2) > 1.0e-12) {
      throw std::runtime_error("Error in duvdy");
    }
#endif

    return tmp2;
  }

  // Evaluates first derivative w.r.t. x for u*w at location of w-component. For details on implementation, see duvdx.
  inline RealType duwdx(const RealType* const lv, const Parameters& parameters, const RealType* const lm) {
#ifndef NDEBUG
    const RealType tmp1 = 1.0 / 4.0 * ((((lv[mapd(0, 0, 0, 0)] + lv[mapd(0, 0, 1, 0)]) *
        (lv[mapd(0, 0, 0, 2)] + lv[mapd(1, 0, 0, 2)])) -
        ((lv[mapd(-1, 0, 0, 0)] + lv[mapd(-1, 0, 1, 0)]) *
            (lv[mapd(-1, 0, 0, 2)] + lv[mapd(0, 0, 0, 2)]))) +
        parameters.solver.gamma * ((fabs(lv[mapd(0, 0, 0, 0)] + lv[mapd(0, 0, 1, 0)]) *
            (lv[mapd(0, 0, 0, 2)] - lv[mapd(1, 0, 0, 2)])) -
            (fabs(lv[mapd(-1, 0, 0, 0)] + lv[mapd(-1, 0, 1, 0)]) *
                (lv[mapd(-1, 0, 0, 2)] - lv[mapd(0, 0, 0, 2)])))) /
        lm[mapd(0, 0, 0, 0)];
#endif

    const RealType hxShort = 0.5 * lm[mapd(0, 0, 0, 0)]; // Distance of corner points in x-direction from center v-value
    const RealType hxLong0 = 0.5 * (lm[mapd(0, 0, 0, 0)] + lm[mapd(-1, 0, 0, 0)]); // Distance between center and west
                                                                                   // v-value
    const RealType hxLong1 = 0.5 * (lm[mapd(0, 0, 0, 0)] + lm[mapd(1, 0, 0, 0)]);  // Distance between center and east
                                                                                   // v-value
    const RealType hzShort = 0.5 * lm[mapd(0, 0, 0, 2)]; // Distance of center u-value from upper edge of cell
    const RealType hzLong = 0.5 * (lm[mapd(0, 0, 0, 2)] + lm[mapd(0, 0, 1, 2)]); // Distance of north and center u-value

    const RealType u00 = lv[mapd(0, 0, 0, 0)];
    const RealType u01 = lv[mapd(0, 0, 1, 0)];
    const RealType w00 = lv[mapd(0, 0, 0, 2)];
    const RealType w10 = lv[mapd(1, 0, 0, 2)];

    const RealType uM10 = lv[mapd(-1, 0, 0, 0)];
    const RealType uM11 = lv[mapd(-1, 0, 1, 0)];
    const RealType wM10 = lv[mapd(-1, 0, 0, 2)];

    const RealType secondOrder = (((hzLong - hzShort) / hzLong * u00 + hzShort / hzLong * u01
                                  ) * ((hxLong1 - hxShort) / hxLong1 * w00 + hxShort / hxLong1 * w10)
                                  - ((hzLong - hzShort) / hzLong * uM10 + hzShort / hzLong * uM11
                                    ) * ((hxLong0 - hxShort) / hxLong0 * w00 + hxShort / hxLong0 * wM10))
                                 / (2.0 * hxShort);

    const RealType kr = (hzLong - hzShort) / hzLong * u00 + hzShort / hzLong * u01;
    const RealType kl = (hzLong - hzShort) / hzLong * uM10 + hzShort / hzLong * uM11;

    const RealType firstOrder
      = 1.0 / (4.0 * hxShort)
        * (kr * (w00 + w10) - kl * (wM10 + w00) + fabs(kr) * (w00 - w10) - fabs(kl) * (wM10 - w00));

    const RealType tmp2 = (1.0 - parameters.solver.gamma) * secondOrder + parameters.solver.gamma * firstOrder;

#ifndef NDEBUG
    if (fabs(tmp1 - tmp2) > 1.0e-12) {
      throw std::runtime_error("Error in duwdx");
    }
#endif

    return tmp2;
  }

  // Evaluates first derivative w.r.t. z for u*w at location of u-component. For details on implementation, see duvdx.
  inline RealType duwdz(const RealType* const lv, const Parameters& parameters, const RealType* const lm) {
#ifndef NDEBUG
    const RealType tmp1 = 1.0 / 4.0 * ((((lv[mapd(0, 0, 0, 2)] + lv[mapd(1, 0, 0, 2)]) *
        (lv[mapd(0, 0, 0, 0)] + lv[mapd(0, 0, 1, 0)])) -
        ((lv[mapd(0, 0, -1, 2)] + lv[mapd(1, 0, -1, 2)]) *
            (lv[mapd(0, 0, -1, 0)] + lv[mapd(0, 0, 0, 0)]))) +
        parameters.solver.gamma * ((fabs(lv[mapd(0, 0, 0, 2)] + lv[mapd(1, 0, 0, 2)]) *
            (lv[mapd(0, 0, 0, 0)] - lv[mapd(0, 0, 1, 0)])) -
            (fabs(lv[mapd(0, 0, -1, 2)] + lv[mapd(1, 0, -1, 2)]) *
                (lv[mapd(0, 0, -1, 0)] - lv[mapd(0, 0, 0, 0)])))) /
        lm[mapd(0, 0, 0, 2)];
#endif

    const RealType hzShort = 0.5 * lm[mapd(0, 0, 0, 2)]; // Distance of corner points in x-direction from center v-value
    const RealType hzLong0 = 0.5 * (lm[mapd(0, 0, 0, 2)] + lm[mapd(0, 0, -1, 2)]); // Distance between center and west
                                                                                   // v-value
    const RealType hzLong1 = 0.5 * (lm[mapd(0, 0, 0, 2)] + lm[mapd(0, 0, 1, 2)]);  // Distance between center and east
                                                                                   // v-value
    const RealType hxShort = 0.5 * lm[mapd(0, 0, 0, 0)]; // Distance of center u-value from upper edge of cell
    const RealType hxLong = 0.5 * (lm[mapd(0, 0, 0, 0)] + lm[mapd(1, 0, 0, 0)]); // Distance of north and center u-value

    const RealType w00 = lv[mapd(0, 0, 0, 2)];
    const RealType w10 = lv[mapd(1, 0, 0, 2)];
    const RealType u00 = lv[mapd(0, 0, 0, 0)];
    const RealType u01 = lv[mapd(0, 0, 1, 0)];

    const RealType w0M1 = lv[mapd(0, 0, -1, 2)];
    const RealType w1M1 = lv[mapd(1, 0, -1, 2)];
    const RealType u0M1 = lv[mapd(0, 0, -1, 0)];

    const RealType secondOrder = (((hxLong - hxShort) / hxLong * w00 + hxShort / hxLong * w10
                                  ) * ((hzLong1 - hzShort) / hzLong1 * u00 + hzShort / hzLong1 * u01)
                                  - ((hxLong - hxShort) / hxLong * w0M1 + hxShort / hxLong * w1M1
                                    ) * ((hzLong0 - hzShort) / hzLong0 * u00 + hzShort / hzLong0 * u0M1))
                                 / (2.0 * hzShort);

    const RealType kr = (hxLong - hxShort) / hxLong * w00 + hxShort / hxLong * w10;
    const RealType kl = (hxLong - hxShort) / hxLong * w0M1 + hxShort / hxLong * w1M1;

    const RealType firstOrder
      = 1.0 / (4.0 * hzShort)
        * (kr * (u00 + u01) - kl * (u0M1 + u00) + fabs(kr) * (u00 - u01) - fabs(kl) * (u0M1 - u00));

    const RealType tmp2 = (1.0 - parameters.solver.gamma) * secondOrder + parameters.solver.gamma * firstOrder;

#ifndef NDEBUG
    if (fabs(tmp1 - tmp2) > 1.0e-12) {
      throw std::runtime_error("Error in duwdz");
    }
#endif

    return tmp2;
  }

  // Evaluates first derivative w.r.t. y for v*w at location of w-component. For details on implementation, see duvdx.
  inline RealType dvwdy(const RealType* const lv, const Parameters& parameters, const RealType* const lm) {
#ifndef NDEBUG
    const RealType tmp1 = 1.0 / 4.0 * ((((lv[mapd(0, 0, 0, 1)] + lv[mapd(0, 0, 1, 1)]) *
        (lv[mapd(0, 0, 0, 2)] + lv[mapd(0, 1, 0, 2)])) -
        ((lv[mapd(0, -1, 0, 1)] + lv[mapd(0, -1, 1, 1)]) *
            (lv[mapd(0, -1, 0, 2)] + lv[mapd(0, 0, 0, 2)]))) +
        parameters.solver.gamma * ((fabs(lv[mapd(0, 0, 0, 1)] + lv[mapd(0, 0, 1, 1)]) *
            (lv[mapd(0, 0, 0, 2)] - lv[mapd(0, 1, 0, 2)])) -
            (fabs(lv[mapd(0, -1, 0, 1)] + lv[mapd(0, -1, 1, 1)]) *
                (lv[mapd(0, -1, 0, 2)] - lv[mapd(0, 0, 0, 2)])))) /
        lm[mapd(0, 0, 0, 1)];
#endif

    const RealType hyShort = 0.5 * lm[mapd(0, 0, 0, 1)]; // Distance of corner points in x-direction from center v-value
    const RealType hyLong0 = 0.5 * (lm[mapd(0, 0, 0, 1)] + lm[mapd(0, -1, 0, 1)]); // Distance between center and west
                                                                                   // v-value
    const RealType hyLong1 = 0.5 * (lm[mapd(0, 0, 0, 1)] + lm[mapd(0, 1, 0, 1)]);  // Distance between center and east
                                                                                   // v-value
    const RealType hzShort = 0.5 * lm[mapd(0, 0, 0, 2)]; // Distance of center u-value from upper edge of cell
    const RealType hzLong = 0.5 * (lm[mapd(0, 0, 0, 2)] + lm[mapd(0, 0, 1, 2)]); // Distance of north and center u-value

    const RealType v00 = lv[mapd(0, 0, 0, 1)];
    const RealType v01 = lv[mapd(0, 0, 1, 1)];
    const RealType w00 = lv[mapd(0, 0, 0, 2)];
    const RealType w10 = lv[mapd(0, 1, 0, 2)];

    const RealType vM10 = lv[mapd(0, -1, 0, 1)];
    const RealType vM11 = lv[mapd(0, -1, 1, 1)];
    const RealType wM10 = lv[mapd(0, -1, 0, 2)];

    const RealType secondOrder = (((hzLong - hzShort) / hzLong * v00 + hzShort / hzLong * v01
                                  ) * ((hyLong1 - hyShort) / hyLong1 * w00 + hyShort / hyLong1 * w10)
                                  - ((hzLong - hzShort) / hzLong * vM10 + hzShort / hzLong * vM11
                                    ) * ((hyLong0 - hyShort) / hyLong0 * w00 + hyShort / hyLong0 * wM10))
                                 / (2.0 * hyShort);

    const RealType kr = (hzLong - hzShort) / hzLong * v00 + hzShort / hzLong * v01;
    const RealType kl = (hzLong - hzShort) / hzLong * vM10 + hzShort / hzLong * vM11;

    const RealType firstOrder
      = 1.0 / (4.0 * hyShort)
        * (kr * (w00 + w10) - kl * (wM10 + w00) + fabs(kr) * (w00 - w10) - fabs(kl) * (wM10 - w00));

    const RealType tmp2 = (1.0 - parameters.solver.gamma) * secondOrder + parameters.solver.gamma * firstOrder;

#ifndef NDEBUG
    if (fabs(tmp1 - tmp2) > 1.0e-12) {
      throw std::runtime_error("Error in dvwdy");
    }
#endif

    return tmp2;
  }

  // Evaluates first derivative w.r.t. z for v*w at location of v-component. For details on implementation, see duvdx.
  inline RealType dvwdz(const RealType* const lv, const Parameters& parameters, const RealType* const lm) {
#ifndef NDEBUG
    const RealType tmp1 = 1.0 / 4.0 * ((((lv[mapd(0, 0, 0, 2)] + lv[mapd(0, 1, 0, 2)]) *
        (lv[mapd(0, 0, 0, 1)] + lv[mapd(0, 0, 1, 1)])) -
        ((lv[mapd(0, 0, -1, 2)] + lv[mapd(0, 1, -1, 2)]) *
            (lv[mapd(0, 0, -1, 1)] + lv[mapd(0, 0, 0, 1)]))) +
        parameters.solver.gamma * ((fabs(lv[mapd(0, 0, 0, 2)] + lv[mapd(0, 1, 0, 2)]) *
            (lv[mapd(0, 0, 0, 1)] - lv[mapd(0, 0, 1, 1)])) -
            (fabs(lv[mapd(0, 0, -1, 2)] + lv[mapd(0, 1, -1, 2)]) *
                (lv[mapd(0, 0, -1, 1)] - lv[mapd(0, 0, 0, 1)])))) /
        lm[mapd(0, 0, 0, 2)];
#endif

    const RealType hzShort = 0.5 * lm[mapd(0, 0, 0, 2)]; // Distance of corner points in x-direction from center v-value
    const RealType hzLong0 = 0.5 * (lm[mapd(0, 0, 0, 2)] + lm[mapd(0, 0, -1, 2)]); // Distance between center and west
                                                                                   // v-value
    const RealType hzLong1 = 0.5 * (lm[mapd(0, 0, 0, 2)] + lm[mapd(0, 0, 1, 2)]);  // Distance between center and east
                                                                                   // v-value
    const RealType hyShort = 0.5 * lm[mapd(0, 0, 0, 1)]; // Distance of center u-value from upper edge of cell
    const RealType hyLong = 0.5 * (lm[mapd(0, 0, 0, 1)] + lm[mapd(0, 1, 0, 1)]); // Distance of north and center u-value

    const RealType w00 = lv[mapd(0, 0, 0, 2)];
    const RealType w10 = lv[mapd(0, 1, 0, 2)];
    const RealType v00 = lv[mapd(0, 0, 0, 1)];
    const RealType v01 = lv[mapd(0, 0, 1, 1)];

    const RealType w0M1 = lv[mapd(0, 0, -1, 2)];
    const RealType w1M1 = lv[mapd(0, 1, -1, 2)];
    const RealType v0M1 = lv[mapd(0, 0, -1, 1)];

    const RealType secondOrder = (((hyLong - hyShort) / hyLong * w00 + hyShort / hyLong * w10
                                  ) * ((hzLong1 - hzShort) / hzLong1 * v00 + hzShort / hzLong1 * v01)
                                  - ((hyLong - hyShort) / hyLong * w0M1 + hyShort / hyLong * w1M1
                                    ) * ((hzLong0 - hzShort) / hzLong0 * v00 + hzShort / hzLong0 * v0M1))
                                 / (2.0 * hzShort);

    const RealType kr = (hyLong - hyShort) / hyLong * w00 + hyShort / hyLong * w10;
    const RealType kl = (hyLong - hyShort) / hyLong * w0M1 + hyShort / hyLong * w1M1;

    const RealType firstOrder
      = 1.0 / (4.0 * hzShort)
        * (kr * (v00 + v01) - kl * (v0M1 + v00) + fabs(kr) * (v00 - v01) - fabs(kl) * (v0M1 - v00));

    const RealType tmp2 = (1.0 - parameters.solver.gamma) * secondOrder + parameters.solver.gamma * firstOrder;

#ifndef NDEBUG
    if (fabs(tmp1 - tmp2) > 1.0e-12) {
      spdlog::error("{}, {}", tmp1, tmp2);
      throw std::runtime_error("Error in dvwdz");
    }
#endif

    return tmp2;
  }

  // First derivative of u*u w.r.t. x, evaluated at location of u-component.
  inline RealType du2dx(const RealType* const lv, const Parameters& parameters, const RealType* const lm) {
#ifndef NDEBUG
    const RealType tmp1 = 1.0 / 4.0 * ((((lv[mapd(0, 0, 0, 0)] + lv[mapd(1, 0, 0, 0)]) *
        (lv[mapd(0, 0, 0, 0)] + lv[mapd(1, 0, 0, 0)])) -
        ((lv[mapd(-1, 0, 0, 0)] + lv[mapd(0, 0, 0, 0)]) *
            (lv[mapd(-1, 0, 0, 0)] + lv[mapd(0, 0, 0, 0)]))) +
        parameters.solver.gamma * ((fabs(lv[mapd(0, 0, 0, 0)] + lv[mapd(1, 0, 0, 0)]) *
            (lv[mapd(0, 0, 0, 0)] - lv[mapd(1, 0, 0, 0)])) -
            (fabs(lv[mapd(-1, 0, 0, 0)] + lv[mapd(0, 0, 0, 0)]) *
                (lv[mapd(-1, 0, 0, 0)] - lv[mapd(0, 0, 0, 0)])))) /
        lm[mapd(0, 0, 0, 0)];
#endif

    const RealType dxShort = 0.5 * lm[mapd(0, 0, 0, 0)];
    // const RealType dxLong0 = 0.5*(lm[mapd(-1,0,0,0)] + lm[mapd(0,0,0,0)]);
    const RealType dxLong1 = 0.5 * (lm[mapd(0, 0, 0, 0)] + lm[mapd(1, 0, 0, 0)]);

    const RealType u0  = lv[mapd(0, 0, 0, 0)];
    const RealType uM1 = lv[mapd(-1, 0, 0, 0)];
    const RealType u1  = lv[mapd(1, 0, 0, 0)];

    // const RealType kr = (dxLong1 - dxShort) / dxLong1 * u0 + dxShort / dxLong1 * u1;
    // const RealType kl = (dxLong0 - dxShort) / dxLong0 * u0 + dxShort / dxLong0 * uM1;
    const RealType kr = (u0 + u1) / 2;
    const RealType kl = (u0 + uM1) / 2;

    // Central difference expression which is second-order accurate for uniform meshes. We interpolate u half-way
    // between neighboured u-component values and afterwards build the central difference for u*u.

    /*const RealType secondOrder = (((dxLong1 - dxShort) / dxLong1 * u0 + dxShort / dxLong1 * u1) * ((dxLong1 - dxShort)
       / dxLong1 * u0 + dxShort / dxLong1 * u1)
        - ((dxLong0 - dxShort) / dxLong0 * u0 + dxShort / dxLong0 * uM1) * ((dxLong0 - dxShort) / dxLong0 * u0 + dxShort
       / dxLong0 * uM1) ) / (2.0 * dxShort);*/

    const RealType secondOrder = ((u0 + u1) * (u0 + u1) - (u0 + uM1) * (u0 + uM1)) / (4 * dxLong1);

    // Donor-cell like derivative expression. We evaluate u half-way between neighboured u-components and use this as a
    // prediction of the transport direction.
    const RealType firstOrder = 1.0 / (4.0 * dxShort)
                                * (kr * (u0 + u1) - kl * (uM1 + u0) + fabs(kr) * (u0 - u1) - fabs(kl) * (uM1 - u0));

    // Return linear combination of central- and upwind difference
    const RealType tmp2 = (1.0 - parameters.solver.gamma) * secondOrder + parameters.solver.gamma * firstOrder;

#ifndef NDEBUG
    if (fabs(tmp1 - tmp2) > 1.0e-12) {
      spdlog::error("{}, {}", tmp1, tmp2);
      throw std::runtime_error("Error in du2dx");
    }
#endif

    return tmp2;
  }

  // First derivative of v*v w.r.t. y, evaluated at location of v-component. For details, see du2dx.
  inline RealType dv2dy(const RealType* const lv, const Parameters& parameters, const RealType* const lm) {
#ifndef NDEBUG
    const RealType tmp1 = 1.0 / 4.0 * ((((lv[mapd(0, 0, 0, 1)] + lv[mapd(0, 1, 0, 1)]) *
        (lv[mapd(0, 0, 0, 1)] + lv[mapd(0, 1, 0, 1)])) -
        ((lv[mapd(0, -1, 0, 1)] + lv[mapd(0, 0, 0, 1)]) *
            (lv[mapd(0, -1, 0, 1)] + lv[mapd(0, 0, 0, 1)]))) +
        parameters.solver.gamma * ((fabs(lv[mapd(0, 0, 0, 1)] + lv[mapd(0, 1, 0, 1)]) *
            (lv[mapd(0, 0, 0, 1)] - lv[mapd(0, 1, 0, 1)])) -
            (fabs(lv[mapd(0, -1, 0, 1)] + lv[mapd(0, 0, 0, 1)]) *
                (lv[mapd(0, -1, 0, 1)] - lv[mapd(0, 0, 0, 1)])))) /
        lm[mapd(0, 0, 0, 1)];
#endif

    const RealType dyShort = 0.5 * lm[mapd(0, 0, 0, 1)];
    // const RealType dyLong0 = 0.5*(lm[mapd(0,-1,0,1)] + lm[mapd(0,0,0,1)]);
    const RealType dyLong1 = 0.5 * (lm[mapd(0, 0, 0, 1)] + lm[mapd(0, 1, 0, 1)]);

    const RealType v0  = lv[mapd(0, 0, 0, 1)];
    const RealType vM1 = lv[mapd(0, -1, 0, 1)];
    const RealType v1  = lv[mapd(0, 1, 0, 1)];

    // const RealType kr = (dyLong1 - dyShort) / dyLong1 * v0 + dyShort / dyLong1 * v1;
    // const RealType kl = (dyLong0 - dyShort) / dyLong0 * v0 + dyShort / dyLong0 * vM1;
    const RealType kr = (v0 + v1) / 2;
    const RealType kl = (v0 + vM1) / 2;

    /*const RealType secondOrder = (((dyLong1 - dyShort) / dyLong1 * v0 + dyShort / dyLong1 * v1) * ((dyLong1 - dyShort)
       / dyLong1 * v0 + dyShort / dyLong1 * v1)
        - ((dyLong0 - dyShort) / dyLong0 * v0 + dyShort / dyLong0 * vM1) * ((dyLong0 - dyShort) / dyLong0 * v0 + dyShort
       / dyLong0 * vM1) ) / (2.0 * dyShort);*/

    const RealType secondOrder = ((v0 + v1) * (v0 + v1) - (v0 + vM1) * (v0 + vM1)) / (4 * dyLong1);

    const RealType firstOrder = 1.0 / (4.0 * dyShort)
                                * (kr * (v0 + v1) - kl * (vM1 + v0) + fabs(kr) * (v0 - v1) - fabs(kl) * (vM1 - v0));

    const RealType tmp2 = (1.0 - parameters.solver.gamma) * secondOrder + parameters.solver.gamma * firstOrder;

#ifndef NDEBUG
    if (fabs(tmp1 - tmp2) > 1.0e-12) {
      spdlog::error("{}, {}", tmp1, tmp2);
      throw std::runtime_error("Error in dv2dy");
    }
#endif

    return tmp2;
  }

  // First derivative of w*w w.r.t. z, evaluated at location of w-component. For details, see du2dx.
  inline RealType dw2dz(const RealType* const lv, const Parameters& parameters, const RealType* const lm) {
#ifndef NDEBUG
    const RealType tmp1 = 1.0 / 4.0 * ((((lv[mapd(0, 0, 0, 2)] + lv[mapd(0, 0, 1, 2)]) *
        (lv[mapd(0, 0, 0, 2)] + lv[mapd(0, 0, 1, 2)])) -
        ((lv[mapd(0, 0, -1, 2)] + lv[mapd(0, 0, 0, 2)]) *
            (lv[mapd(0, 0, -1, 2)] + lv[mapd(0, 0, 0, 2)]))) +
        parameters.solver.gamma * ((fabs(lv[mapd(0, 0, 0, 2)] + lv[mapd(0, 0, 1, 2)]) *
            (lv[mapd(0, 0, 0, 2)] - lv[mapd(0, 0, 1, 2)])) -
            (fabs(lv[mapd(0, 0, -1, 2)] + lv[mapd(0, 0, 0, 2)]) *
                (lv[mapd(0, 0, -1, 2)] - lv[mapd(0, 0, 0, 2)])))) /
        lm[mapd(0, 0, 0, 2)];
#endif

    const RealType dzShort = 0.5 * lm[mapd(0, 0, 0, 2)];
    // const RealType dzLong0 = 0.5 * (lm[mapd(0, 0, -1, 2)] + lm[mapd(0, 0, 0, 2)]);
    const RealType dzLong1 = 0.5 * (lm[mapd(0, 0, 0, 2)] + lm[mapd(0, 0, 1, 2)]);

    const RealType w0  = lv[mapd(0, 0, 0, 2)];
    const RealType wM1 = lv[mapd(0, 0, -1, 2)];
    const RealType w1  = lv[mapd(0, 0, 1, 2)];

    // const RealType kr = (dzLong1 - dzShort) / dzLong1 * w0 + dzShort / dzLong1 * w1;
    // const RealType kl = (dzLong0 - dzShort) / dzLong0 * w0 + dzShort / dzLong0 * wM1;
    const RealType kr = (w0 + w1) / 2;
    const RealType kl = (w0 + wM1) / 2;

    /*const RealType secondOrder = (((dzLong1 - dzShort) / dzLong1 * w0 + dzShort / dzLong1 * w1) * ((dzLong1 - dzShort)
       / dzLong1 * w0 + dzShort / dzLong1 * w1)
        - ((dzLong0 - dzShort) / dzLong0 * w0 + dzShort / dzLong0 * wM1) * ((dzLong0 - dzShort) / dzLong0 * w0 + dzShort
       / dzLong0 * wM1) ) / (2.0 * dzShort);*/

    const RealType secondOrder = ((w0 + w1) * (w0 + w1) - (w0 + wM1) * (w0 + wM1)) / (4 * dzLong1);

    const RealType firstOrder = 1.0 / (4.0 * dzShort)
                                * (kr * (w0 + w1) - kl * (wM1 + w0) + fabs(kr) * (w0 - w1) - fabs(kl) * (wM1 - w0));

    const RealType tmp2 = (1.0 - parameters.solver.gamma) * secondOrder + parameters.solver.gamma * firstOrder;

#ifndef NDEBUG
    if (fabs(tmp1 - tmp2) > 1.0e-12) {
      spdlog::error("{}, {}", tmp1, tmp2);
      throw std::runtime_error("Error in dw2dz");
    }
#endif

    return tmp2;
  }

  inline RealType computeF2D(
    const RealType* const localVelocity, const RealType* const localMeshsize, const Parameters& parameters, RealType dt
  ) {
    // TODO WS1
    RealType viscousTermU = d2udx2(localVelocity, localMeshsize); // d2u/dx2
    RealType viscousTermV = d2udy2(localVelocity, localMeshsize); // d2u/dy2
    return localVelocity[mapd(0, 0, 0, 0)]
        + dt * (-du2dx(localVelocity, parameters, localMeshsize) 
                - duvdy(localVelocity, parameters, localMeshsize)
                + 1/parameters.flow.Re*(viscousTermU + viscousTermV) 
                + parameters.environment.gx);
  }

  inline RealType computeG2D(
    const RealType* const localVelocity, const RealType* const localMeshsize, const Parameters& parameters, RealType dt
  ) {
    // TODO WS1
    RealType viscousTermU = d2vdx2(localVelocity, localMeshsize); // d2v/dx2
    RealType viscousTermV = d2vdy2(localVelocity, localMeshsize); // d2v/dy2
    return localVelocity[mapd(0, 0, 0, 1)]
        + dt * (- duvdx(localVelocity, parameters, localMeshsize) 
                - dv2dy(localVelocity, parameters, localMeshsize)
                +  1/parameters.flow.Re*(viscousTermU + viscousTermV) 
                + parameters.environment.gy);
  }

  inline RealType computeF3D(
    const RealType* const localVelocity, const RealType* const localMeshsize, const Parameters& parameters, RealType dt
  ) {
    // TODO WS1
    RealType viscousTermU = d2udx2(localVelocity, localMeshsize); // d²u/dx²
    RealType viscousTermV = d2udy2(localVelocity, localMeshsize); // d²u/dy²
    RealType viscousTermW = d2udz2(localVelocity, localMeshsize); // d²u/dz²
    return localVelocity[mapd(0, 0, 0, 0)]
        + dt * (-du2dx(localVelocity, parameters, localMeshsize) 
                - duvdy(localVelocity, parameters, localMeshsize)
                - duwdz(localVelocity, parameters, localMeshsize) 
                + 1/parameters.flow.Re*(viscousTermU + viscousTermV + viscousTermW)
                + parameters.environment.gx);
  }

  inline RealType computeG3D(
    const RealType* const localVelocity, const RealType* const localMeshsize, const Parameters& parameters, RealType dt
  ) {
    // TODO WS1
    RealType viscousTermU = d2vdx2(localVelocity, localMeshsize); // d²v/dx²
    RealType viscousTermV = d2vdy2(localVelocity, localMeshsize); // d²v/dy²
    RealType viscousTermW = d2vdz2(localVelocity, localMeshsize); // d²v/dz²
    return localVelocity[mapd(0, 0, 0, 1)]
        + dt * (-dSv2dy(localVelocity, parameters, localMeshsize) 
                - duvdx(localVelocity, parameters, localMeshsize)
                - dvwdz(localVelocity, parameters, localMeshsize) 
                + 1/parameters.flow.Re*(viscousTermU + viscousTermV + viscousTermW)
                + parameters.environment.gy);
  }

  inline RealType computeH3D(
    const RealType* const localVelocity, const RealType* const localMeshsize, const Parameters& parameters, RealType dt
  ) {
    // TODO WS1
    RealType viscousTermU = d2wdx2(localVelocity, localMeshsize); // d²w/dx²
    RealType viscousTermV = d2wdy2(localVelocity, localMeshsize); // d²w/dy²
    RealType viscousTermW = d2wdz2(localVelocity, localMeshsize); // d²w/dz²
    return localVelocity[mapd(0, 0, 0, 2)]
        + dt * (-dw2dz(localVelocity, parameters, localMeshsize) 
                - duwdx(localVelocity, parameters, localMeshsize)
                - dvwdy(localVelocity, parameters, localMeshsize) 
                + 1/parameters.flow.Re*(viscousTermU + viscousTermV + viscousTermW)
                + parameters.environment.gz);
  }

} // namespace Stencils
