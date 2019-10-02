#pragma once

#include <AK/String.h>
#include <AK/HashMap.h>
#include <AK/Vector.h>
#include <LibGUI/GModel.h>
#include <unistd.h>

class GraphWidget;

class ProcessModel final : public GModel {
public:
    enum Column {
        Icon = 0,
        Name,
        CPU,
        State,
        Priority,
        User,
        PID,
        Virtual,
        Physical,
        Syscalls,
        InodeFaults,
        ZeroFaults,
        CowFaults,
        __Count
    };

    static ProcessModel& the();

    static NonnullRefPtr<ProcessModel> create() { return adopt(*new ProcessModel); }
    virtual ~ProcessModel() override;

    virtual int row_count(const GModelIndex&) const override;
    virtual int column_count(const GModelIndex&) const override;
    virtual String column_name(int column) const override;
    virtual ColumnMetadata column_metadata(int column) const override;
    virtual GVariant data(const GModelIndex&, Role = Role::Display) const override;
    virtual void update() override;

    Function<void(float)> on_new_cpu_data_point;

private:
    ProcessModel();

    struct ProcessState {
        pid_t pid;
        unsigned times_scheduled;
        String name;
        String state;
        String user;
        String priority;
        size_t amount_virtual;
        size_t amount_resident;
        unsigned syscall_count;
        unsigned inode_faults;
        unsigned zero_faults;
        unsigned cow_faults;
        float cpu_percent;
        int icon_id;
    };

    struct Process {
        ProcessState current_state;
        ProcessState previous_state;
    };

    HashMap<uid_t, String> m_usernames;
    HashMap<pid_t, NonnullOwnPtr<Process>> m_processes;
    Vector<pid_t> m_pids;
    RefPtr<GraphicsBitmap> m_generic_process_icon;
    RefPtr<GraphicsBitmap> m_high_priority_icon;
    RefPtr<GraphicsBitmap> m_low_priority_icon;
    RefPtr<GraphicsBitmap> m_normal_priority_icon;
};
