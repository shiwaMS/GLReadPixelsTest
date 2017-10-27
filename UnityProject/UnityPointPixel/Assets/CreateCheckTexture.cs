using UnityEngine;
using System;
using System.Runtime.InteropServices;

public class CreateCheckTexture: MonoBehaviour
{
	[DllImport("ScreenPointPixel-lib")]
	private static extern void create_check_texture(IntPtr data, int w, int h, int ch);
	[DllImport("ScreenPointPixel-lib")]
	private static extern void setTextureId(Int32 textureId);

	private Texture2D texture_;
	private Color32[] pixels_;
	private GCHandle pixels_handle_;
	private IntPtr pixels_ptr_ = IntPtr.Zero;

	void Start()
	{
		// テクスチャを生成
		texture_ = new Texture2D(10, 10, TextureFormat.RGB24, false);
		// テクスチャの拡大方法をニアレストネイバーに変更
		texture_.filterMode = FilterMode.Point;
		// Color32 型の配列としてテクスチャの参照をもらう
		pixels_ = texture_.GetPixels32();
		// GC されないようにする
		pixels_handle_ = GCHandle.Alloc(pixels_, GCHandleType.Pinned);
		// そのテクスチャのアドレスをもらう
		pixels_ptr_ = pixels_handle_.AddrOfPinnedObject();
		// スクリプトがアタッチされたオブジェクトのテクスチャをコレにする
		GetComponent<Renderer>().material.mainTexture = texture_;
        IntPtr nativeTextureId = texture_.GetNativeTexturePtr();
        Debug.Log("CreateCheckTexture native texture: " + nativeTextureId);
		// ネイティブ側でテクスチャを生成
		create_check_texture(pixels_ptr_, texture_.width, texture_.height, 4);
        setTextureId(nativeTextureId.ToInt32());
        nativeTextureId = texture_.GetNativeTexturePtr();
        Debug.Log("CreateCheckTexture native texture: " + nativeTextureId);
		// セットして反映させる
		texture_.SetPixels32(pixels_);
		texture_.Apply(false, true);
		// GC 対象にする
		pixels_handle_.Free();
	}
}
