// based on https://github.com/mmozeiko/c_d2d_dwrite/blob/main/Generator/Generator.cs

using System;
using System.IO.Compression;
using Mono.Cecil;

var fields	= new HashSet<FieldDefinition>();
var typedefs	= new HashSet<TypeDefinition>();
var enums	= new HashSet<TypeDefinition>();
var structs	= new HashSet<TypeDefinition>();
var interfaces	= new HashSet<TypeDefinition>();
var functions	= new HashSet<MethodDefinition>();

string GetParamType(ParameterDefinition arg)
{
    string str = "";
    if(arg.CustomAttributes.FirstOrDefault(attr => attr.AttributeType.FullName == "Windows.Win32.Foundation.Metadata.ConstAttribute") != null)
    {
	str += "const ";
    }
    str += GetSimpleType(arg.ParameterType);
    return(str);
}

string GetSimpleType(TypeReference type)
{
    if(type.IsPointer)
    {
	var element_type = ((TypeSpecification)type).ElementType;
	return(GetSimpleType(element_type) + "*");
    }
    if(type.MetadataType == MetadataType.Void)
    {
	return("void");
    }
    if(type.FullName == "System.GUID")
    {
	return("GUID");
    }
    if(type.Name == "PWSTR")
    {
	return("WCHAR*");
    }
    if(type.IsPrimitive)
    {
	switch(type.MetadataType)
	{
	    case MetadataType.SByte:	return "INT8";
	    case MetadataType.Byte:	return "UINT8";
	    case MetadataType.Int16:	return "INT16";
	    case MetadataType.UInt16:	return "UINT16";
	    case MetadataType.Int32:	return "INT32";
	    case MetadataType.UInt32:	return "UINT32";
	    case MetadataType.Int64:	return "INT64";
	    case MetadataType.UInt64:	return "UINT64";
	    case MetadataType.Single:	return "FLOAT";
	    case MetadataType.Double:	return "DOUBLE";
	    default: throw new NotSupportedException();
	}
    }
    if(type.Resolve().IsInterface)
    {
	return(type.Name + "*");
    }

    return(type.Name);
}

void WriteEnums()
{
    foreach(var e in enums.OrderBy(e => e.Name))
    {
	Console.WriteLine($"enum {e.Name}");
    }
}

void WriteStructs()
{
    foreach(var s in structs.OrderBy(s => s.Name))
    {
	Console.WriteLine($"struct {s.Name}");
    }
}

void WriteInterfaces()
{
    int max_len = interfaces.Max(i => i.Name.Length + 1);

    foreach(var i in interfaces.OrderBy(i => i.Name))
    {
	//Console.WriteLine($"interface {i.Name}");
	Console.WriteLine($"typedef struct {i.Name.PadRight(max_len)} {{ struct {{ void *tbl[]; }} *v; }} {i.Name};");
    }
}

void WriteFunctions()
{
    Console.WriteLine();
    Console.WriteLine("// functions");
    Console.WriteLine();

    int max_ret = functions.Max(f => GetSimpleType(f.ReturnType).Length);
    int max_len = functions.Max(f => f.Name.Length + 1);

    foreach(var f in functions.OrderBy(f => f.Name))
    {
	//Console.WriteLine($"function {f.Name}");
	// NOTE: ignoring some functions that are probably not needed for now, so that the code runs
	if(Char.IsUpper(f.Name[0]))
	{
	    Console.Write($"EXTERN_C {GetSimpleType(f.ReturnType).PadRight(max_ret)} DECLSPEC_IMPORT WINAPI {f.Name.PadRight(max_len)}(");
	    Console.Write(string.Join(", ", f.Parameters.Select(arg => $"{GetParamType(arg)} {arg.Name}")));
	    Console.Write(") WIN_NOEXCEPT;");
	    Console.WriteLine();
	}
    }
}

void PrintAllTypesInNamespace(ModuleDefinition module, string ns)
{
    foreach(var type in module.GetTypes().Where(type => type.Namespace == ns))
    {
	Console.WriteLine($"{type}");
    }
}

void Parse(ModuleDefinition module)
{
    foreach(var type in module.GetTypes()
	     .Where(type => type.Namespace == "Windows.Win32.Media.Audio")
	     //.OrderBy(type => type.Name)
    )
    {
	var name = type.Name;
	//Console.WriteLine($"{name}");
	if(name == "Apis")
	{
	    foreach(var field in type.Fields)
	    {
		//Console.WriteLine($"field {field}");
		fields.Add(field);
	    }
	    foreach(var method in type.Methods)
	    {
		//Console.WriteLine($"  {method}");
		functions.Add(method);
	    }
	}
	else if(type.IsInterface)
	{
	    //Console.WriteLine($"{name}");
	    interfaces.Add(type);
	}
	else if(type.IsEnum)
	{
	    enums.Add(type);
	}
	else if(type.IsClass && type.IsValueType)
	{
	    structs.Add(type);
	}
    }

    WriteInterfaces();
    WriteEnums();
    WriteStructs();
    WriteFunctions();
}

var fs = File.OpenRead("../../../../data/win32-metadata/Windows.Win32.winmd.gz");
var gz = new GZipStream(fs, CompressionMode.Decompress);
var stream = new MemoryStream();
gz.CopyTo(stream);
stream.Seek(0, SeekOrigin.Begin);
var module = ModuleDefinition.ReadModule(stream);
//PrintAllTypesInNamespace(module, "Windows.Win32.Media.Audio");
Parse(module);
