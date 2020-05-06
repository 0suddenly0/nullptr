#pragma once

#include <string>

enum send_prop_type
{
    DPT_Int = 0,
    DPT_Float,
    DPT_Vector,
    DPT_VectorXY,
    DPT_String,
    DPT_Array,
    DPT_DataTable,
    DPT_Int64,
    DPT_NUMSendPropTypes
};

class d_variant
{
public:
    union
    {
        float	   m_float;
        long	   m_int;
        char*    m_string;
        void*    m_data;
        float	   m_vector[3];
        __int64  m_int64;
    };
    send_prop_type  m_type;
};

class recv_table;
class recv_prop;

class c_recv_proxy_data
{
public:
    const recv_prop*     recv_prop;        // The property it's receiving.
    d_variant		    value;            // The value given to you to store.
    int				    element;         // Which array element you're getting.
    int				    object_id;         // The object being referred to.
};

//-----------------------------------------------------------------------------
// pStruct = the base structure of the datatable this variable is in (like c_base_entity)
// pOut    = the variable that this this proxy represents (like c_base_entity::m_SomeValue).
//
// Convert the network-standard-type value in m_Value into your own format in pStruct/pOut.
//-----------------------------------------------------------------------------
typedef void(*recv_var_proxy_fn)(const c_recv_proxy_data *pData, void *pStruct, void *pOut);

// ------------------------------------------------------------------------ //
// ArrayLengthRecvProxies are optionally used to Get the length of the 
// incoming array when it changes.
// ------------------------------------------------------------------------ //
typedef void(*array_length_recv_proxy_fn)(void *pStruct, int objectID, int currentArrayLength);

// NOTE: DataTable receive proxies work differently than the other proxies.
// pData points at the object + the recv table's offset.
// pOut should be Set to the location of the object to unpack the data table into.
// If the parent object just contains the child object, the default proxy just does *pOut = pData.
// If the parent object points at the child object, you need to dereference the pointer here.
// NOTE: don't ever return null from a DataTable receive proxy function. Bad things will happen.
typedef void(*data_table_recv_var_proxy_fn)(const recv_prop *pProp, void **pOut, void *pData, int objectID);

class recv_prop
{
public:
    char*                           var_name;
    send_prop_type                  recv_type;
    int                             flags;
    int                             string_buffer_size;
    int                             inside_array;
    const void*                     extra_data;
    recv_prop*                      array_prop;
    array_length_recv_proxy_fn      array_length_proxy;
    recv_var_proxy_fn               proxy_fn;
    data_table_recv_var_proxy_fn    data_table_proxy_fn;
    recv_table*                     data_table;
    int                             offset;
    int                             element_stride;
    int                             elements;
    const char*                     parent_array_prop_name;

    recv_var_proxy_fn		        get_proxy_fn() const;
    void					        set_proxy_fn(recv_var_proxy_fn fn);
    data_table_recv_var_proxy_fn	get_data_table_proxy_fn() const;
    void					        set_data_table_proxy_fn(data_table_recv_var_proxy_fn fn);

};

class recv_table
{
public:
    recv_prop*              p_props;
    int                     n_props;
    void*                   decoder;
    char*                   net_table_name;
    bool                    initialized;
    bool                    in_main_list;
};

recv_var_proxy_fn recv_prop::get_proxy_fn() const
{
    return proxy_fn;
}

void recv_prop::set_proxy_fn(recv_var_proxy_fn fn)
{
    proxy_fn = fn;
}

data_table_recv_var_proxy_fn recv_prop::get_data_table_proxy_fn() const
{
    return data_table_proxy_fn;
}

void recv_prop::set_data_table_proxy_fn(data_table_recv_var_proxy_fn fn)
{
    data_table_proxy_fn = fn;
}

class recv_prop_hook
{
public:
	recv_prop_hook(recv_prop* prop, const recv_var_proxy_fn proxy_fn) : m_property(prop), m_original_proxy_fn(prop->proxy_fn)
	{
		set_proxy_function(proxy_fn);
	}
	~recv_prop_hook()
	{
		m_property->proxy_fn = m_original_proxy_fn;
	}
	auto get_original_function() const -> recv_var_proxy_fn
	{
		return m_original_proxy_fn;
	}
	auto set_proxy_function(const recv_var_proxy_fn proxy_fn) const -> void
	{
		m_property->proxy_fn = proxy_fn;
	}
private:
	recv_prop* m_property;
	recv_var_proxy_fn m_original_proxy_fn;
};