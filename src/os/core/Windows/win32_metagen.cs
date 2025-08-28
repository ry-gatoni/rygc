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

void WriteField(int level, int max_len, FieldDefinition field)
{
    string indent = string.Empty.PadRight(2*level);
    
    if(field.FieldType.IsArray)
    {
	var array_type = (ArrayType)field.FieldType;
	Console.WriteLine($"{indent}{GetSimpleType(array_type.ElementType).PadRight(max_len)} {field.Name}[{array_type.Dimensions[0].UpperBound + 1}];");
    }
    else
    {
	var type = field.FieldType.Resolve();
	if(type.IsNested)
	{
	    max_len = type.Fields
		.Where(nested => !nested.FieldType.Resolve().IsNested)
		.Aggregate(0, (max, nested) => Math.Max(max, GetSimpleType(nested.FieldType).Length));
	    
	    Console.WriteLine($"{indent}struct");
	    Console.WriteLine($"{indent}{{");
	    foreach(var nested_field in type.Fields)
	    {
		WriteField(level + 1, max_len, nested_field);
	    }
	    Console.WriteLine($"{indent}}} {field.Name};");
	}
	else
	{
	    Console.WriteLine($"{indent}{GetSimpleType(field.FieldType).PadRight(max_len)} {field.Name};");
	}
    }
}

void WriteEnums()
{
    Console.WriteLine();
    Console.WriteLine("// enums");

    foreach(var e in enums.OrderBy(e => e.Name))
    {
	Console.WriteLine();

	int max_len = e.Fields.Where(field => !field.IsSpecialName).Max(field => field.Name.Length);

	var name = e.Name;
	Console.WriteLine($"typedef enum {name}");
	Console.WriteLine($"{{");
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
	    Console.WriteLine($"  {field.Name.PadRight(max_len)} = {val},");
	}
	Console.WriteLine($"}} {name};");
    }
}

void WriteStructs()
{
    Console.WriteLine();
    Console.WriteLine("// structs");

    foreach(var s in structs.OrderBy(s => s.Name))
    {
	Console.WriteLine();
	
	int max_len = s.Fields
	    .Where(field => !field.FieldType.Resolve().IsNested)
	    .Aggregate(0, (max, field) => Math.Max(max, GetSimpleType(field.FieldType).Length));

	var typename = (s.Attributes & TypeAttributes.ExplicitLayout) != 0 ? "union" : "struct";
	var name = s.Name;
	Console.WriteLine($"typedef {typename} {name}");
	Console.WriteLine($"{{");
	foreach(var field in s.Fields)
	{
	    WriteField(1, max_len, field);
	}
	Console.WriteLine($"}} {name};");
    }
}

void WriteInterfaces()
{
    Console.WriteLine();
    Console.WriteLine("// interfaces");
    Console.WriteLine();

    int max_len = interfaces.Max(i => i.Name.Length + 1);

    foreach(var i in interfaces.OrderBy(i => i.Name))
    {
	//Console.WriteLine($"interface {i.Name}");
	Console.WriteLine($"typedef struct {i.Name.PadRight(max_len)} {{ struct {{ void *tbl[]; }} *v; }} {i.Name};");
    }
}

void WriteMethodsRecursive(string base_type, TypeDefinition type, HashSet<string> names, ref uint index)
{
    if(type.Interfaces.Count != 0)
    {
	WriteMethodsRecursive(base_type, type.Interfaces[0].InterfaceType.Resolve(), names, ref index);
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
	Console.Write($"static inline {ret_type} {name}(base_type *this_");
	if(member.Parameters.Count != 0)
	{
	    Console.Write(", ");
	}
	Console.Write(string.Join(", ", member.Parameters.Select(arg => $"{GetParamType(arg)} {arg.Name}")));
	Console.Write($") {{ ");
	if(ret_type != "void")
	{
	    Console.Write("return(");
	}
	Console.Write($"(({ret_type} (WINAPI*)({base_type}*");
	if(member.Parameters.Count != 0)
	{
	    Console.Write(", ");
	}
	Console.Write(string.Join(", ", member.Parameters.Select(arg => GetParamType(arg))));
	Console.Write($"))this_->v->tbl[{index}](this_");
	if(member.Parameters.Count != 0)
	{
	    Console.Write(", ");
	}
	Console.Write(string.Join(", ", member.Parameters.Select(arg => arg.Name)));
	Console.WriteLine($")); }}");
	index++;
    }
}

void WriteMethods()
{
    Console.WriteLine();
    Console.WriteLine("// methods");

    foreach(var i in interfaces.OrderBy(i => i.Name))
    {
	Console.WriteLine();
	var names = new HashSet<string>();
	uint index = 0;
	WriteMethodsRecursive(i.Name, i, names, ref index);
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
    WriteMethods();
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
