using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Windows;
using System.IO;

public class ExtractMeshByTextWithNormal : MonoBehaviour
{
    private StreamWriter streamWriter = null;
    private int m_nFrames = 0;

    const float EPSILON = 1.0e-6f;

    bool IsZero(float fValue) { return ((Mathf.Abs(fValue) < EPSILON)); }
    bool IsEqual(float fA, float fB) { return (IsZero(fA - fB)); }

    void WriteTabs(int nLevel)
    {
        for (int i = 0; i < nLevel; i++) streamWriter.Write("\t");
    }

     void WriteObjectName(int nLevel, string strHeader, Object obj)
    {
        WriteTabs(nLevel);
        streamWriter.Write(strHeader + " ");
        streamWriter.WriteLine((obj) ? string.Copy(obj.name).Replace(" ", "_") : "null");
    }

   void WriteString(string strToWrite)
    {
        streamWriter.Write(strToWrite);
    }

    void WriteString(int nLevel, string strToWrite)
    {
        WriteTabs(nLevel);
        streamWriter.Write(strToWrite);
    }

    void WriteLineString(string strToWrite)
    {
        streamWriter.WriteLine(strToWrite);
    }

    void WriteLineString(int nLevel, string strToWrite)
    {
        WriteTabs(nLevel);
        streamWriter.WriteLine(strToWrite);
    }

    void WriteVector(Vector2 v)
    {
        streamWriter.Write(v.x + " " + v.y + " ");
    }

    void WriteVector(string strHeader, Vector2 v)
    {
        streamWriter.Write(strHeader);
        WriteVector(v);
    }

    void WriteVector(Vector3 v)
    {
        streamWriter.Write(v.x + " " + v.y + " " + v.z + " ");
    }

    void WriteVector(string strHeader, Vector3 v)
    {
        streamWriter.Write(strHeader);
        WriteVector(v);
    }

    void WriteVector(Vector4 v)
    {
        streamWriter.Write(v.x + " " + v.y + " " + v.z + " " + v.w + " ");
    }

    void WriteVector(string strHeader, Vector4 v)
    {
        streamWriter.Write(strHeader);
        WriteVector(v);
    }

    void WriteVector(Quaternion q)
    {
        streamWriter.Write(q.x + " " + q.y + " " + q.z + " " + q.w + " ");
    }

    void WriteVector(string strHeader, Quaternion q)
    {
        streamWriter.Write(strHeader);
        WriteVector(q);
    }

    void WriteVectors(int nLevel, string strHeader, Vector2[] vectors)
    {
        WriteString(nLevel, strHeader + " " + vectors.Length + " ");
        if (vectors.Length > 0)
        {
            foreach (Vector2 v in vectors) streamWriter.Write(v.x + " " + v.y + " ");
        }
        streamWriter.WriteLine(" ");
    }

    void WriteVectors(int nLevel, string strHeader, Vector3[] vectors)
    {
        WriteString(nLevel, strHeader + " " + vectors.Length + " ");
        if (vectors.Length > 0)
        {
            foreach (Vector3 v in vectors) streamWriter.Write(v.x + " " + v.y + " " + v.z + " ");
        }
        streamWriter.WriteLine(" ");
    }

    void WriteVectors(int nLevel, string strHeader, Vector4[] vectors)
    {
        WriteString(nLevel, strHeader + " " + vectors.Length + " ");
        if (vectors.Length > 0)
        {
            foreach (Vector4 v in vectors) streamWriter.Write(v.x + " " + v.y + " " + v.z + " " + v.w + " ");
        }
        streamWriter.WriteLine(" ");
    }

    void WriteColors(int nLevel, string strHeader, Color[] colors)
    {
        WriteString(nLevel, strHeader + " " + colors.Length + " ");
        if (colors.Length > 0)
        {
            foreach (Color c in colors) streamWriter.Write(c.r + " " + c.g + " " + c.b + " " + c.a + " ");
        }
        streamWriter.WriteLine(" ");
    }

    void WriteIntegers(int nLevel, string strHeader, int[] integers)
    {
        WriteString(nLevel, strHeader + " " + integers.Length + " ");
        if (integers.Length > 0)
        {
            foreach (int i in integers) streamWriter.Write(i + " ");
        }
        streamWriter.WriteLine(" ");
    }

    void WriteMatrix(Matrix4x4 matrix)
    {
        streamWriter.Write(matrix.m00 + " " + matrix.m10 + " " + matrix.m20 + " " + matrix.m30 + " ");
        streamWriter.Write(matrix.m01 + " " + matrix.m11 + " " + matrix.m21 + " " + matrix.m31 + " ");
        streamWriter.Write(matrix.m02 + " " + matrix.m12 + " " + matrix.m22 + " " + matrix.m32 + " ");
        streamWriter.Write(matrix.m03 + " " + matrix.m13 + " " + matrix.m23 + " " + matrix.m33 + " ");
    }

    void WriteMatrix(Vector3 position, Quaternion rotation, Vector3 scale)
    {
        Matrix4x4 matrix = Matrix4x4.identity;
        matrix.SetTRS(position, rotation, scale);
        WriteMatrix(matrix);
    }

    void WriteTransform(int nLevel, string strHeader, Transform current)
    {
        WriteString(nLevel, strHeader + " ");
        WriteVector(current.localPosition);
        WriteVector(current.localEulerAngles);
        WriteVector(current.localScale);
        WriteVector(current.localRotation);
        streamWriter.WriteLine(" ");
    }

    void WriteLocalMatrix(int nLevel, string strHeader, Transform current)
    {
        WriteString(nLevel, strHeader + " ");
        Matrix4x4 matrix = Matrix4x4.identity;
        matrix.SetTRS(current.localPosition, current.localRotation, current.localScale);
        WriteMatrix(matrix);
        streamWriter.WriteLine(" ");
    }

    void WriteWorldMatrix(int nLevel, string strHeader, Transform current)
    {
        WriteString(nLevel, strHeader + " ");
        Matrix4x4 matrix = Matrix4x4.identity;
        matrix.SetTRS(current.position, current.rotation, current.lossyScale);
        WriteMatrix(matrix);
        streamWriter.WriteLine(" ");
    }

    void WriteBoneTransforms(int nLevel, string strHeader, Transform[] bones)
    {
        WriteString(nLevel, strHeader + " " + bones.Length + " ");
        if (bones.Length > 0)
        {
            foreach (Transform bone in bones)
            {
                WriteMatrix(bone.localPosition, bone.localRotation, bone.localScale);
            }
        }
        streamWriter.WriteLine(" ");
    }

    void WriteBoneNames(int nLevel, string strHeader, Transform[] bones)
    {
        WriteString(nLevel, strHeader + " " + bones.Length + " ");
        if (bones.Length > 0)
        {
            foreach (Transform transform in bones)
            {
                streamWriter.Write(string.Copy(transform.gameObject.name).Replace(" ", "_") + " ");
            }
        }
        streamWriter.WriteLine(" ");
    }

    void WriteMatrixes(int nLevel, string strHeader, Matrix4x4[] matrixes)
    {
        WriteString(nLevel, strHeader + " " + matrixes.Length + " ");
        if (matrixes.Length > 0)
        {
            foreach (Matrix4x4 matrix in matrixes)
            {
                WriteMatrix(matrix);
            }
        }
        streamWriter.WriteLine(" ");
    }

    void WriteMeshInfo(int nLevel, Mesh mesh)
    {
        WriteObjectName(nLevel, "<Mesh>: " + mesh.vertexCount, mesh);

        WriteLineString(nLevel+1, "<Bounds>: " + mesh.bounds.center.x + " " + mesh.bounds.center.y + " " + mesh.bounds.center.z + " " + mesh.bounds.extents.x + " " + mesh.bounds.extents.y + " " + mesh.bounds.extents.z);

        WriteVectors(nLevel+1, "<Positions>:", mesh.vertices);
        WriteColors(nLevel+1, "<Colors>:", mesh.colors);
        WriteVectors(nLevel+1, "<Normals>:", mesh.normals);

        WriteLineString(nLevel+1, "<SubMeshes>: " + mesh.subMeshCount);
        if (mesh.subMeshCount > 0)
        {
            for (int i = 0; i < mesh.subMeshCount; i++)
            {
                int[] subindicies = mesh.GetTriangles(i);
                WriteIntegers(nLevel+2, "<SubMesh>: " + i, subindicies);
            }
        }

        WriteLineString(nLevel, "</Mesh>");
    }

    void WriteMaterials(int nLevel, Material[] materials)
    {
        WriteLineString(nLevel, "<Materials>: " + materials.Length);
        if (materials.Length > 0)
        {
            for (int i = 0; i < materials.Length; i++)
            {
                WriteLineString(nLevel+1, "<Material>: " + i);

                if (materials[i].HasProperty("_Color"))
                {
                    Color albedo = materials[i].GetColor("_Color");
                    WriteLineString(nLevel+2, "<AlbedoColor>: " + albedo.r + " " + albedo.g + " " + albedo.b + " " + albedo.a);
                }
                if (materials[i].HasProperty("_EmissionColor"))
                {
                    Color emission = materials[i].GetColor("_EmissionColor");
                    WriteLineString(nLevel+2, "<EmissiveColor>: " + emission.r + " " + emission.g + " " + emission.b + " " + emission.a);
                }
                if (materials[i].HasProperty("_SpecColor"))
                {
                    Color specular = materials[i].GetColor("_SpecColor");
                    WriteLineString(nLevel+2, "<SpecularColor>: " + specular.r + " " + specular.g + " " + specular.b + " " + specular.a);
                }
                if (materials[i].HasProperty("_Glossiness"))
                {
                    WriteLineString(nLevel+2, "<Glossiness>: " + materials[i].GetFloat("_Glossiness"));
                }
                if (materials[i].HasProperty("_Smoothness"))
                {
                    WriteLineString(nLevel+2, "<Smoothness>: " + materials[i].GetFloat("_Smoothness"));
                }
                if (materials[i].HasProperty("_Metallic"))
                {
                    WriteLineString(nLevel+2, "<Metallic>: " + materials[i].GetFloat("_Metallic"));
                }
                if (materials[i].HasProperty("_SpecularHighlights"))
                {
                    WriteLineString(nLevel+2, "<SpecularHighlight>: " + materials[i].GetFloat("_SpecularHighlights"));
                }
                if (materials[i].HasProperty("_GlossyReflections"))
                {
                    WriteLineString(nLevel+2, "<GlossyReflection>: " + materials[i].GetFloat("_GlossyReflections"));
                }
            }
        }
        WriteLineString(nLevel, "</Materials>");
    }

    void WriteFrameInfo(int nLevel, Transform current)
    {
        if (current.gameObject.activeSelf)
        {
            WriteObjectName(nLevel, "<Frame>: " + m_nFrames++, current.gameObject);

            WriteTransform(nLevel + 1, "<Transform>:", current);
            WriteLocalMatrix(nLevel + 1, "<TransformMatrix>:", current);

            MeshFilter meshFilter = current.gameObject.GetComponent<MeshFilter>();
            MeshRenderer meshRenderer = current.gameObject.GetComponent<MeshRenderer>();

            if (meshFilter && meshRenderer)
            {
                WriteMeshInfo(nLevel + 1, meshFilter.sharedMesh);

                Material[] materials = meshRenderer.materials;
                if (materials.Length > 0) WriteMaterials(nLevel + 1, materials);
            }
            else
            {
                SkinnedMeshRenderer skinMeshRenderer = current.gameObject.GetComponent<SkinnedMeshRenderer>();
                if (skinMeshRenderer)
                {
                    WriteMeshInfo(nLevel + 1, skinMeshRenderer.sharedMesh);

                    Material[] materials = skinMeshRenderer.materials;
                    if (materials.Length > 0) WriteMaterials(nLevel + 1, materials);
                }
            }
        }
    }

    void WriteFrameNameHierarchy(Transform current)
    {
        streamWriter.Write(string.Copy(current.gameObject.name).Replace(" ", "_") + " ");

        if (current.childCount > 0)
        {
            for (int k = 0; k < current.childCount; k++) WriteFrameNameHierarchy(current.GetChild(k));
        }
    }

    void WriteFrameNames(int nLevel, string strHeader)
    {
        WriteString(nLevel, strHeader);
        WriteFrameNameHierarchy(transform);
        streamWriter.WriteLine(" ");
    }

    void WriteFrameHierarchyInfo(int nLevel, Transform current)
    {
        WriteFrameInfo(nLevel, current);

        WriteLineString(nLevel+1, "<Children>: " + current.childCount);

        if (current.childCount > 0)
        {
            for (int k = 0; k < current.childCount; k++) WriteFrameHierarchyInfo(nLevel+2, current.GetChild(k));
        }

        WriteLineString(nLevel, "</Frame>");
    }

    void Start()
    {
        streamWriter = new StreamWriter(string.Copy(gameObject.name).Replace(" ", "_") + ".txt");

		WriteLineString("<Hierarchy>:");
        WriteFrameHierarchyInfo(1, transform);
		WriteLineString("</Hierarchy>");
   
        streamWriter.Flush();
        streamWriter.Close();

        print("Model Text Write Completed");
    }
}

