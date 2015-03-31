/*

Copyright 2015 Aleksander Berg-Jones

This file is part of Shadow's Spider.

Shadow's Spider is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Shadow's Spider is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Shadow's Spider.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "Gizmo.h"

/*
reference:
    https://github.com/CedricGuillemet/LibGizmo
*/

Gizmo::Gizmo(MainWin &myWinTemp) : myWin(myWinTemp) { }

string Gizmo::hover(shared_ptr<GLWidget> myGLin)
{
    myGL = myGLin;

    string gizType = myWin.myGizNull->gizType;

    //BUILD INITIAL RAY
    myGL->giz_rayDir = rayCast(myGL->pNew);
    myGL->giz_rayOrigin = glm::vec3(myGL->selCamLi->t->val_3);

    if (gizType == "R")
    {
        glm::vec3 axX(1.f, 0.f, 0.f);
        glm::vec3 axY(0.f, 1.f, 0.f);
        glm::vec3 axZ(0.f, 0.f, 1.f);

        if (myWin.gizSpace == "local" && myWin.selB)
        {
            axX = glm::normalize(glm::vec3(myWin.selB->MM * glm::vec4(axX, 0.f)));
            axY = glm::normalize(glm::vec3(myWin.selB->MM * glm::vec4(axY, 0.f)));
            axZ = glm::normalize(glm::vec3(myWin.selB->MM * glm::vec4(axZ, 0.f)));
        }

        if (rotatePlane(axX, myGL->gizHoverCheckX)) return "X";
        else if (rotatePlane(axY, myGL->gizHoverCheckY)) return "Y";
        else if (rotatePlane(axZ, myGL->gizHoverCheckZ)) return "Z";
    }

    else if (gizType == "T" || gizType == "S")
    {
        float dualStart = 0.f;
        float dualEnd = .5f;
        float xyzEnd = .2f;

        glm::vec3 df = rayInter(glm::vec3(0.f, 1.f, 0.f), 1, "full"); //XZ

        if (gizType == "S" && df.x > 0 && df.x < xyzEnd && df.z > 0 && df.z < xyzEnd) return "XYZ";
        else if (df.x >= 0 && df.x <= 1 && fabs(df.z) < .1f) return "X";
        else if (df.z >= 0 && df.z <= 1 && fabs(df.x) < .1f) return "Z";
        else if (df.x > dualStart && df.x < dualEnd && df.z > dualStart && df.z < dualEnd) return "XZ";

        else
        {
            df = rayInter(glm::vec3(0.f, 0.f, 1.f), 1, "full"); //XY

            if (gizType == "S" && df.x > 0 && df.x < xyzEnd && df.y > 0 && df.y < xyzEnd) return "XYZ";
            else if (df.x >= 0 && df.x <= 1 && fabs(df.y) < .1f) return "X";
            else if (df.y >= 0 && df.y <= 1 && fabs(df.x) < .1f) return "Y";
            else if (df.x > dualStart && df.x < dualEnd && df.y > dualStart && df.y < dualEnd) return "XY";

            else
            {
                df = rayInter(glm::vec3(1.f, 0.f, 0.f), 1, "full"); //YZ

                if (gizType == "S" && df.y > 0 && df.y < xyzEnd && df.z > 0 && df.z < xyzEnd) return "XYZ";
                else if (df.y >= 0 && df.y <= 1 && fabs(df.z) < .1f) return "Y";
                else if (df.z >= 0 && df.z <= 1 && fabs(df.y) < .1f) return "Z";
                else if (df.y > dualStart && df.y < dualEnd && df.z > dualStart && df.z < dualEnd) return "YZ";
            }
        }
    }

    return "NONE";
}

void Gizmo::transform(shared_ptr<GLWidget> myGLin)
{
    myGL = myGLin;

    //BUILD NEW RAY FOR DRAG
    myGL->giz_rayDir = rayCast(myGL->pNew);
    myGL->giz_rayOrigin = glm::vec3(myGL->selCamLi->t->val_3);

    //RAY INTERSECT
    glm::vec3 axX(1.f, 0.f, 0.f);
    glm::vec3 axY(0.f, 1.f, 0.f);
    glm::vec3 axZ(0.f, 0.f, 1.f);

    glm::vec3 df = rayInter(myGL->gizN, 0, "inter") - myGL->gizMouseDown;
    glm::vec3 adf;

    if (myWin.myGizNull->gizType == "T")
    {
        if (myWin.gizSpace == "local")
        {
            axX = glm::normalize(glm::vec3(myWin.selB->MM * glm::vec4(axX, 0.f)));
            axY = glm::normalize(glm::vec3(myWin.selB->MM * glm::vec4(axY, 0.f)));
            axZ = glm::normalize(glm::vec3(myWin.selB->MM * glm::vec4(axZ, 0.f)));
        }

        df = dotInterAx(myGL->pD, df, axX, axY, axZ, myGL->gizTransType);
        adf = df.x * axX + df.y * axY + df.z * axZ;

        for (unsigned int i = 0; i < myWin.allObj.size(); ++i)
        {
            if (myWin.allObj[i]->selected)
                myWin.allObj[i]->t->val_3 += adf;
        }
    }

    else if (myWin.myGizNull->gizType == "R")
    {
        glm::vec4 clipP = myGL->selCamLi->PM * (myGL->selCamLi->VM * glm::vec4(myWin.selB->t->val_3, 1.f));

        glm::vec3 ndcP = glm::vec3(clipP.x, clipP.y, clipP.z)  / clipP.w;
        float viewOffset = 0.f;
        glm::vec2 ssP = ((glm::vec2(ndcP.x, ndcP.y) + 1.f) / 2.f) * glm::vec2(myGL->width(), myGL->height()) + viewOffset;

        glm::vec3 camFor = glm::vec3(myGL->selCamLi->MM * glm::vec4(0.f, 0.f, 1.f, 0.f));

        //get screen space direction of mouse move vs mouse xy delta
        float sign = glm::sign(glm::dot(camFor, myGL->gizN));
        if (sign == 0.f) sign = 1.f; // looking for 1 or -1

        glm::vec2 mouseDelta = glm::normalize(myGL->pNew - ssP);
        glm::vec3 screenDir = glm::normalize(sign * glm::vec3(-glm::sign(mouseDelta.y), glm::sign(mouseDelta.x), 0.f));

        glm::vec3 pDiff(myGL->pD.x, myGL->pD.y, 0.f);
        float mouseDeltaInAxisDir = glm::dot(screenDir, pDiff); //

        glm::vec2 pOld2(myGL->pOld.x, myGL->pOld.y);
        glm::vec3 rayDirAfter = rayCast(pOld2 + mouseDeltaInAxisDir * glm::vec2(screenDir));
        glm::vec3 rayDirBefore = rayCast(pOld2);

        float gotEditDelta = glm::length(rayDirAfter - rayDirBefore) * glm::sign(mouseDeltaInAxisDir) * 360.f;

        for (unsigned int i = 0; i < myWin.allObj.size(); ++i)
        {
            if (myWin.allObj[i]->selected)
            {
                if (myGL->gizTransType == "X")
                    myWin.allObj[i]->r->val_3.x += -gotEditDelta;

                if (myGL->gizTransType == "Y")
                    myWin.allObj[i]->r->val_3.y += -gotEditDelta;

                if (myGL->gizTransType == "Z")
                    myWin.allObj[i]->r->val_3.z += -gotEditDelta;
            }
        }
    }

    else if (myWin.myGizNull->gizType == "S")
    {
//        if (myWin.gizSpace == "local")
//        {
//            axX = glm::normalize(glm::vec3(myWin.selB->MM * glm::vec4(axX, 0.f)));
//            axY = glm::normalize(glm::vec3(myWin.selB->MM * glm::vec4(axY, 0.f)));
//            axZ = glm::normalize(glm::vec3(myWin.selB->MM * glm::vec4(axZ, 0.f)));
//        }

        adf = dotInterAx(myGL->pD, df, axX, axY, axZ, myGL->gizTransType);

        float dualSpeed = 25.f;

        if (myGL->gizTransType == "XZ")
        {
            float dualAdd = int(df.x + df.z * dualSpeed) / dualSpeed;
            adf = glm::vec3(dualAdd, 0.f, dualAdd);
        }

        else if (myGL->gizTransType == "XY")
        {
            float dualAdd = int(df.x + df.y * dualSpeed) / dualSpeed;
            adf = glm::vec3(dualAdd, dualAdd, 0.f);
        }

        else if (myGL->gizTransType == "YZ")
        {
            float dualAdd = int(df.y + df.z * dualSpeed) / dualSpeed;
            adf = glm::vec3(0.f, dualAdd, dualAdd);
        }

        for (unsigned int i = 0; i < myWin.allObj.size(); ++i)
        {
            if (myWin.allObj[i]->selected)
                myWin.allObj[i]->s->val_3 += adf;
        }
    }

    myGL->gizMouseDown += adf;

    myWin.setLightsDirty();
}

glm::vec3 Gizmo::dotInterAx(glm::vec2 pD, glm::vec3 df, glm::vec3 axX, glm::vec3 axY, glm::vec3 axZ, string type)
{
    if (type == "XYZ") return glm::vec3(pD.x * .01f);
    else if (type == "X") return glm::vec3(glm::dot(df, axX), 0.f, 0.f);
    else if (type == "Y") return glm::vec3(0.f, glm::dot(df, axY), 0.f);
    else if (type == "Z") return glm::vec3(0.f, 0.f, glm::dot(df, axZ));
    else if (type == "XZ") return glm::vec3(glm::dot(df, axX), 0.f, glm::dot(df, axZ));
    else if (type == "XY") return glm::vec3(glm::dot(df, axX), glm::dot(df, axY), 0.f);
    else if (type == "YZ") return glm::vec3(0.f, glm::dot(df, axY), glm::dot(df, axZ));

    return glm::vec3(0.f);
}

glm::vec3 Gizmo::rayCast2(glm::vec2 XYin)
{
    qDebug() << "rayCast2 debug";

    glm::vec4 lRayEnd_NDC((XYin.x / myGL->width() - .5f) * 2.f, (XYin.y / myGL->height() - .5f) * -2.f, 0.f, 1.f);

    glm::vec4 lRayEnd_world = glm::inverse(myGL->selCamLi->PM * myGL->selCamLi->VM) * lRayEnd_NDC;
    lRayEnd_world /= lRayEnd_world.w;

    glm::vec3 lRayDir_world(lRayEnd_world - glm::vec4(myGL->selCamLi->t->val_3, 1.f));

    return glm::normalize(lRayDir_world);

    //    if (myGL->selCamLi->camLiTypeGet("dirOrtho")) // DEBUG ORTHO RAYCAST
    //    {
    //        glm::vec3 dirN = myGL->giz_rayDir - myGL->giz_rayOrigin;
    //        myGL->giz_rayDir = glm::normalize(dirN);
    //    }
}

glm::vec3 Gizmo::rayCast(glm::vec2 XYin)
{
    glm::vec4 lRayEnd_NDC((XYin.x / myGL->width() - .5f) * 2.f, (XYin.y / myGL->height() - .5f) * -2.f, 0.f, 1.f);

    glm::vec4 lRayEnd_world = glm::inverse(myGL->selCamLi->PM * myGL->selCamLi->VM) * lRayEnd_NDC;
    lRayEnd_world /= lRayEnd_world.w;

    glm::vec3 lRayDir_world(lRayEnd_world - glm::vec4(myGL->selCamLi->t->val_3, 1.f));

    return glm::normalize(lRayDir_world);

//    if (myGL->selCamLi->camLiTypeGet("dirOrtho")) // DEBUG ORTHO RAYCAST
//    {
//        glm::vec3 dirN = myGL->giz_rayDir - myGL->giz_rayOrigin;
//        myGL->giz_rayDir = glm::normalize(dirN);
//    }
}

glm::vec3 Gizmo::rayInter(glm::vec3 normal, bool getTrueNormal, string mode)
{
    if (getTrueNormal)
    {
        glm::vec4 normalAdjusted = myWin.myGizNull->RM * glm::vec4(normal, 1.f);
        normal = glm::vec3(normalAdjusted);
    }

    myGL->gizN = normal;

    //RAY INTERSECT
    glm::vec4 gizPlane = glm::vec4(normal, glm::dot(myWin.myGizNull->parentTo->t->val_3, normal));
    glm::vec3 tmp = (normal * gizPlane.w) - myGL->giz_rayOrigin;
    float den = glm::dot(normal, myGL->giz_rayDir);
    glm::vec3 inter = myGL->giz_rayOrigin + (glm::dot(normal, tmp) / den) * myGL->giz_rayDir;

    if (mode == "inter")
        return inter;

    myGL->gizMouseDown = inter; // only set on the full vers

    return glm::vec3(glm::inverse(myWin.myGizNull->MM) * glm::vec4(inter, 1.f)) / myWin.gizScale; //TRANSFORM PT
}

bool Gizmo::rotatePlane(glm::vec3 normal, glm::vec3 hoverCheck)
{
    glm::vec3 inter = rayInter(normal, 0, "inter");
    myGL->gizMouseDown = inter;

    glm::vec3 df = (inter - myWin.myGizNull->parentTo->t->val_3);

    float dotCheck = glm::dot(df, hoverCheck * myWin.gizScale);
    float hoverThick = .2f;

    if (dotCheck >= 0.f)
    {
        if (glm::length(df) > (1.f - hoverThick) * myWin.gizScale && glm::length(df) < (1.f + hoverThick) * myWin.gizScale)
            return 1;
    }

    return 0;
}

void Gizmo::resetSetCol(string type)
{
    //reset color
    for (unsigned int i = 0; i < myWin.allObj.size(); ++i)
    {
        QString type = myWin.allObj[i]->type;

        if (type == "GIZ_CONE" || type == "GIZ_CUBE" || type == "GIZ_DUAL_HANDLE" || type == "GIZ_LINE" || type == "GIZ_CIRC" || type == "GIZ_CIRC_HALF")
            myWin.allObj[i]->Cgiz = myWin.allObj[i]->Cgiz_stored;
    }

    glm::vec3 Csel = glm::vec3(1.f, 1.f, 0.f); //yellow

    for (unsigned int i = 0; i < myWin.allGiz.size(); ++i)
    {
        QString name = myWin.allGiz[i]->name->val_s;

        if (type == "X" && (name == "gizCircHalfX" || name == "gizConeX" || name == "gizCubeX" || name == "gizLineX"))
            myWin.allGiz[i]->Cgiz = Csel;

        else if (type == "Y" && (name == "gizCircHalfY" || name == "gizConeY" || name == "gizCubeY" || name == "gizLineY"))
            myWin.allGiz[i]->Cgiz = Csel;

        else if (type == "Z" && (name == "gizCircHalfZ" || name == "gizConeZ" || name == "gizCubeZ" || name == "gizLineZ"))
            myWin.allGiz[i]->Cgiz = Csel;

        else if (type == "XYZ" && name == "gizCubeXYZ") myWin.allGiz[i]->Cgiz = Csel;
        else if (type == "XZ" && name == "gizDualXZ") myWin.allGiz[i]->Cgiz = Csel;
        else if (type == "XY" && name == "gizDualXY") myWin.allGiz[i]->Cgiz = Csel;
        else if (type == "YZ" && name == "gizDualYZ") myWin.allGiz[i]->Cgiz = Csel;
    }
}
