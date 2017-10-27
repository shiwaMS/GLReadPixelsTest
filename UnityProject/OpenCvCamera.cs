using UnityEngine;
using System;
using System.Runtime.InteropServices;

public class OpenCvCamera : MonoBehaviour
{
    [DllImport ("ExternalTexture-lib")]
    private static extern IntPtr getCamera(int device);
    [DllImport ("ExternalTexture-lib")]
    private static extern void releaseCamera(IntPtr ptr);
    [DllImport ("ExternalTexture-lib")]
    private static extern int getCameraWidth(IntPtr ptr);
    [DllImport ("ExternalTexture-lib")]
    private static extern int getCameraHeight(IntPtr ptr);
    [DllImport ("ExternalTexture-lib")]
    private static extern void updateCamera(IntPtr ptr);
    [DllImport ("ExternalTexture-lib")]
    private static extern void setCameraTexturePtr(IntPtr ptr, IntPtr texture);

    private IntPtr camera_ = IntPtr.Zero;

    WebCamTexture cameraTex;

    void Start()
    {
        camera_ = getCamera(0);

        var tex = new Texture2D(
            getCameraWidth(camera_),
            getCameraWidth(camera_),
            TextureFormat.RGB24,
            false, /* mipmap */
            true /* linear */);
        GetComponent<Renderer>().material.mainTexture = tex;

        setCameraTexturePtr(camera_, tex.GetNativeTexturePtr());
    }

    void OnDestroy()
    {
        releaseCamera(camera_);
    }

    void Update()
    {
        updateCamera(camera_);
    }
}