#include "netvars.h"

#include <fstream>
#include <utility>

#include "sdk.h"
namespace netvar_sys
{
    void initialize()
    {
        database.clear();

        for (auto client_class = g_chl_client->get_all_classes();
            client_class != nullptr;
            client_class = client_class->next) {
            if (client_class->recv_table) {
                database.emplace_back(load_table(client_class->recv_table));
            }
        }
    }

    netvar_table load_table(recv_table* recvTable)
    {
        auto table = netvar_table{};

        table.offset = 0;
        table.name = recvTable->net_table_name;

        for (auto i = 0; i < recvTable->n_props; ++i) {
            auto prop = &recvTable->p_props[i];

            if (!prop || isdigit(prop->var_name[0]))
                continue;
            if (strcmp("baseclass", prop->var_name) == 0)
                continue;

            if (prop->recv_type == DPT_DataTable && prop->data_table) {
                table.child_tables.emplace_back(load_table(prop->data_table));
                table.child_tables.back().offset = prop->offset;
                table.child_tables.back().prop = prop;
            }
            else {
                table.child_props.emplace_back(prop);
            }
        }
        return table;
    }

    void dump()
    {
        auto outfile = std::ofstream("netvar_dump.txt");

        dump(outfile);
    }

    void dump(std::ostream& stream)
    {
        for (const auto& table : database) {
            if (table.child_props.empty() && table.child_tables.empty())
                continue;
            stream << table.name << '\n';
            dump_table(stream, table, 1);
            stream << '\n';
        }

        stream << std::endl;
    }

    void dump_table(std::ostream& stream, const netvar_table& table, uint32_t indentation)
    {
        char line_buffer[1024];

        for (const auto& prop : table.child_props) {
            sprintf_s(line_buffer, "%*c%*s: 0x%08X", indentation * 4, ' ', -(50 - (int)indentation * 4), prop->var_name, table.offset + prop->offset);
            stream << line_buffer << '\n';
        }
        for (const auto& child : table.child_tables) {
            sprintf_s(line_buffer, "%*c%*s: 0x%08X", indentation * 4, ' ', -(50 - (int)indentation * 4), child.prop->var_name, table.offset + child.offset);
            stream << line_buffer << '\n';
            dump_table(stream, child, indentation + 1);
        }
    }

    uint32_t get_offset(const std::string& tableName, const std::string& propName)
    {
        auto result = 0u;
        for (const auto& table : database) {
            if (table.name == tableName) {
                result = get_offset(table, propName);
                if (result != 0)
                    return result;
            }
        }
        return 0;
    }

    uint32_t get_offset(const netvar_table& table, const std::string& propName)
    {
        for (const auto& prop : table.child_props) {
            if (strncmp(prop->var_name, propName.data(), propName.size()) == 0) {
                return table.offset + prop->offset;
            }
        }
        for (const auto& child : table.child_tables) {
            auto prop_offset = get_offset(child, propName);
            if (prop_offset != 0)
                return table.offset + prop_offset;
        }
        for (const auto& child : table.child_tables) {
            if (strncmp(child.prop->var_name, propName.data(), propName.size()) == 0) {
                return table.offset + child.offset;
            }
        }
        return 0;
    }

    recv_prop* get_netvar_prop(const std::string& tableName, const std::string& propName)
    {
        recv_prop* result = nullptr;
        for (const auto& table : database) {
            if (table.name == tableName) {
                result = get_netvar_prop(table, propName);
            }
        }
        return result;
    }

    recv_prop* get_netvar_prop(const netvar_table& table, const std::string& propName)
    {
        for (const auto& prop : table.child_props) {
            if (strncmp(prop->var_name, propName.data(), propName.size()) == 0) {
                return prop;
            }
        }
        for (const auto& child : table.child_tables) {
            auto prop = get_netvar_prop(child, propName);
            if (prop != 0)
                return prop;
        }
        for (const auto& child : table.child_tables) {
            if (strncmp(child.prop->var_name, propName.data(), propName.size()) == 0) {
                return child.prop;
            }
        }
        return nullptr;
    }
}
