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
	    case MetadataType.Char:	return "CHAR";
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
	    case MetadataType.UIntPtr:  return "DWORD_PTR";
	    default: Console.WriteLine($"{type.MetadataType}"); throw new NotSupportedException();
	}
    }
    if(type.Resolve().IsInterface)
    {
	return(type.Name + "*");
    }

    return(type.Name);
}

void WriteField(StreamWriter fs, int level, int max_len, FieldDefinition field)
{
    string indent = string.Empty.PadRight(2*level);
    
    if(field.FieldType.IsArray)
    {
	var array_type = (ArrayType)field.FieldType;
	fs.WriteLine($"{indent}{GetSimpleType(array_type.ElementType).PadRight(max_len)} {field.Name}[{array_type.Dimensions[0].UpperBound + 1}];");
    }
    else
    {
	var type = field.FieldType.Resolve();
	if(type.IsNested)
	{
	    max_len = type.Fields
		.Where(nested => !nested.FieldType.Resolve().IsNested)
		.Aggregate(0, (max, nested) => Math.Max(max, GetSimpleType(nested.FieldType).Length));
	    
	    fs.WriteLine($"{indent}struct");
	    fs.WriteLine($"{indent}{{");
	    foreach(var nested_field in type.Fields)
	    {
		WriteField(fs, level + 1, max_len, nested_field);
	    }
	    fs.WriteLine($"{indent}}} {field.Name};");
	}
	else
	{
	    fs.WriteLine($"{indent}{GetSimpleType(field.FieldType).PadRight(max_len)} {field.Name};");
	}
    }
}

void WriteGuids(StreamWriter fs)
{
    fs.WriteLine();
    fs.WriteLine("// guids");
    fs.WriteLine();

    int max_len = interfaces.Max(i => i.Name.Length);

    foreach(var i in interfaces.OrderBy(i => i.Name))
    {
	var guid_attr = i.CustomAttributes.FirstOrDefault(attr => attr.AttributeType.FullName == "Windows.Win32.Foundation.Metadata.GuidAttribute");

	var ga = guid_attr.ConstructorArguments;
	var name = $"IID_{i.Name},";
	fs.WriteLine($"DEFINE_GUID({name.PadRight(max_len + 5)} 0x{ga[0].Value:x8}, 0x{ga[1].Value:x4}, 0x{ga[2].Value:x4}, {string.Join(", ", ga.Skip(3).Select(x => $"0x{x.Value:x2}"))});");
    }
}


void WriteEnums(StreamWriter fs)
{
    fs.WriteLine();
    fs.WriteLine("// enums");

    foreach(var e in enums.OrderBy(e => e.Name))
    {
	fs.WriteLine();

	int max_len = e.Fields.Where(field => !field.IsSpecialName).Max(field => field.Name.Length);

	var name = e.Name;
	fs.WriteLine($"typedef enum {name}");
	fs.WriteLine($"{{");
	foreach(var field in e.Fields.Where(field => !field.IsSpecialName))
	{
	    var field_const = field.Constant;
	    string val;
	    if(field_const is int)
	    {
		int int_val = (int)field_const;
		if(int_val < 0)
		{
		    val = $"0x{(uint)int_val:x8}L";
		}
		else
		{
		    val = $"{int_val}";
		}
	    }
	    else if(field_const is uint)
	    {
		val = $"0x{(uint)field_const:x8}";
	    }
	    else
	    {
		throw new InvalidOperationException();
	    }
	    fs.WriteLine($"  {field.Name.PadRight(max_len)} = {val},");
	}
	fs.WriteLine($"}} {name};");
    }
}

void WriteStructs(StreamWriter fs)
{
    fs.WriteLine();
    fs.WriteLine("// structs");

    foreach(var s in structs.OrderBy(s => s.Name))
    {
	fs.WriteLine();
	
	int max_len = s.Fields
	    .Where(field => !field.FieldType.Resolve().IsNested)
	    .Aggregate(0, (max, field) => Math.Max(max, GetSimpleType(field.FieldType).Length));

	var typename = (s.Attributes & TypeAttributes.ExplicitLayout) != 0 ? "union" : "struct";
	var name = s.Name;
	fs.WriteLine($"typedef {typename} {name}");
	fs.WriteLine($"{{");
	foreach(var field in s.Fields)
	{
	    WriteField(fs, 1, max_len, field);
	}
	fs.WriteLine($"}} {name};");
    }
}

void WriteInterfaces(StreamWriter fs)
{
    fs.WriteLine();
    fs.WriteLine("// interfaces");
    fs.WriteLine();

    int max_len = interfaces.Max(i => i.Name.Length + 1);

    foreach(var i in interfaces.OrderBy(i => i.Name))
    {
	//Console.WriteLine($"interface {i.Name}");
	fs.WriteLine($"typedef struct {i.Name.PadRight(max_len)} {{ struct {{ void *tbl[]; }} *v; }} {i.Name};");
    }
}

void WriteMethodsRecursive(StreamWriter fs, string base_type, TypeDefinition type, HashSet<string> names, ref uint index)
{
    if(type.Interfaces.Count != 0)
    {
	WriteMethodsRecursive(fs, base_type, type.Interfaces[0].InterfaceType.Resolve(), names, ref index);
    }

    foreach(var member in type.Methods)
    {
	var member_name = member.Name;
	// NOTE: postfix numbers to the ends of methods which are overridden in later interface versions
	if(names.Contains(member_name))
	{
	    int idx = 1;
	    while(true)
	    {
		var new_name = $"{member_name}{idx}";
		if(!names.Contains(new_name))
		{
		    member_name = new_name;
		    break;
		}
		idx++;
	    }
	}
	names.Add(member_name);

	var ret_type = GetSimpleType(member.ReturnType);

	var name = $"{base_type}_{member_name}";
	fs.Write($"static inline {ret_type} {name}(base_type *this_");
	if(member.Parameters.Count != 0)
	{
	    fs.Write(", ");
	}
	fs.Write(string.Join(", ", member.Parameters.Select(arg => $"{GetParamType(arg)} {arg.Name}")));
	fs.Write($") {{ ");
	if(ret_type != "void")
	{
	    fs.Write("return(");
	}
	fs.Write($"(({ret_type} (WINAPI*)({base_type}*");
	if(member.Parameters.Count != 0)
	{
	    fs.Write(", ");
	}
	fs.Write(string.Join(", ", member.Parameters.Select(arg => GetParamType(arg))));
	fs.Write($"))this_->v->tbl[{index}](this_");
	if(member.Parameters.Count != 0)
	{
	    fs.Write(", ");
	}
	fs.Write(string.Join(", ", member.Parameters.Select(arg => arg.Name)));
	fs.WriteLine($")); }}");
	index++;
    }
}

void WriteMethods(StreamWriter fs)
{
    fs.WriteLine();
    fs.WriteLine("// methods");

    foreach(var i in interfaces.OrderBy(i => i.Name))
    {
	fs.WriteLine();
	var names = new HashSet<string>();
	uint index = 0;
	WriteMethodsRecursive(fs, i.Name, i, names, ref index);
    }
}

void WriteFunctions(StreamWriter fs)
{
    fs.WriteLine();
    fs.WriteLine("// functions");
    fs.WriteLine();

    int max_ret = functions.Max(f => GetSimpleType(f.ReturnType).Length);
    int max_len = functions.Max(f => f.Name.Length + 1);

    foreach(var f in functions.OrderBy(f => f.Name))
    {
	//Console.WriteLine($"function {f.Name}");
	// NOTE: ignoring some functions that are probably not needed for now, so that the code runs
	if(Char.IsUpper(f.Name[0]))
	{
	    fs.Write($"EXTERN_C {GetSimpleType(f.ReturnType).PadRight(max_ret)} DECLSPEC_IMPORT WINAPI {f.Name.PadRight(max_len)}(");
	    fs.Write(string.Join(", ", f.Parameters.Select(arg => $"{GetParamType(arg)} {arg.Name}")));
	    fs.Write(") WIN_NOEXCEPT;");
	    fs.WriteLine();
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

void Parse(ModuleDefinition module, string out_path, string ns)
{
    foreach(var type in module.GetTypes()
	    .Where(type => type.Namespace == ns)
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

    using (var fs = File.CreateText(out_path))
    {
	fs.WriteLine("// generated from src/os/windows/win32_metagen.cs");
	fs.WriteLine("#include <initguid.h>");

	WriteGuids(fs);
	WriteInterfaces(fs);
	WriteEnums(fs);
	WriteStructs(fs);
	WriteMethods(fs);
	WriteFunctions(fs);
    }
}

var fs = File.OpenRead("../../../../data/win32-metadata/Windows.Win32.winmd.gz");
var gz = new GZipStream(fs, CompressionMode.Decompress);
var stream = new MemoryStream();
gz.CopyTo(stream);
stream.Seek(0, SeekOrigin.Begin);
var module = ModuleDefinition.ReadModule(stream);
//PrintAllTypesInNamespace(module, "Windows.Win32.Media.Audio");
Parse(module, "win32_audio_defines.h", "Windows.Win32.Media.Audio");
