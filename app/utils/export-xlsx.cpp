//
// Created by dingjing on 23-10-13.
//

#include "export-xlsx.h"


bool ExportXlsx::addBaseInfo(
    const QString &taskID,
    const QString &taskName,
    const QString &computerName,
    const QString &computerIP,
    const QString &computerMAC,
    const QString &scanStartTime,
    const QString &scanStopTime,
    const QString &checkItemCount,
    const QString &successItemCount,
    const QString &warningItemCount,
    const QString &systemName,
    const QString &systemLanguage,
    const QString &systemManufacture,
    const QString &systemModel,
    const QString &bios,
    const QString &cpu,
    const QString &mem,
    const QString &swap,
    const QString &desktop)
{
    if (mDoc.addSheet ("基本信息")) {
        mDoc.selectSheet ("基本信息");
        // header
        {
            mDoc.write (1, 1, "任务ID");
            mDoc.write (2, 1, (((nullptr == taskID) || taskID.isEmpty()) ? "" : taskID));
        }

        {
            mDoc.write (1, 2, "任务名称");
            mDoc.write (2, 2, (((nullptr == taskName) || taskName.isEmpty()) ? "" : taskName));
        }

        {
            mDoc.write (1, 3, "主机名称");
            mDoc.write (2, 3, (((nullptr == computerName) || computerName.isEmpty()) ? "" : computerName));
        }

        {
            mDoc.write (1, 4, "IP地址");
            mDoc.write (2, 4, (((nullptr == computerIP) || computerIP.isEmpty()) ? "" : computerIP));
        }

        {
            mDoc.write (1, 5, "MAC地址");
            mDoc.write (2, 5, (((nullptr == computerMAC) || computerMAC.isEmpty()) ? "" : computerMAC));
        }

        {
            mDoc.write (1, 6, "开始时间");
            mDoc.write (2, 6, (((nullptr == scanStartTime) || scanStartTime.isEmpty()) ? "" : scanStartTime));
        }

        {
            mDoc.write (1, 7, "结束时间");
            mDoc.write (2, 7, (((nullptr == scanStopTime) || scanStopTime.isEmpty()) ? "" : scanStopTime));
        }

        {
            mDoc.write (1, 8, "检查项");
            mDoc.write (2, 8, (((nullptr == checkItemCount) || checkItemCount.isEmpty()) ? "" : checkItemCount));
        }

        {
            mDoc.write (1, 9, "合格项");
            mDoc.write (2, 9, (((nullptr == successItemCount) || successItemCount.isEmpty()) ? "" : successItemCount));
        }

        {
            mDoc.write (1, 10, "不合格项");
            mDoc.write (2, 10, (((nullptr == warningItemCount) || warningItemCount.isEmpty()) ? "" : warningItemCount));
        }

        {
            mDoc.write (1, 11, "操作系统");
            mDoc.write (2, 11, (((nullptr == systemName) || systemName.isEmpty()) ? "" : systemName));
        }

        {
            mDoc.write (1, 12, "语言");
            mDoc.write (2, 12, (((nullptr == systemLanguage) || systemLanguage.isEmpty()) ? "" : systemLanguage));
        }

        {
            mDoc.write (1, 13, "系统制造商");
            mDoc.write (2, 13, (((nullptr == systemManufacture) || systemManufacture.isEmpty()) ? "" : systemManufacture));
        }

        {
            mDoc.write (1, 14, "系统型号");
            mDoc.write (2, 14, (((nullptr == systemModel) || systemModel.isEmpty()) ? "" : systemModel));
        }

        {
            mDoc.write (1, 15, "BIOS");
            mDoc.write (2, 15, (((nullptr == bios) || bios.isEmpty()) ? "" : bios));
        }

        {
            mDoc.write (1, 16, "处理器");
            mDoc.write (2, 16, (((nullptr == cpu) || cpu.isEmpty()) ? "" : cpu));
        }

        {
            mDoc.write (1, 17, "内存");
            mDoc.write (2, 17, (((nullptr == mem) || mem.isEmpty()) ? "" : mem));
        }

        {
            mDoc.write (1, 18, "交换分区");
            mDoc.write (2, 18, (((nullptr == swap) || swap.isEmpty()) ? "" : swap));
        }

        {
            mDoc.write (1, 19, "桌面环境");
            mDoc.write (2, 19, (((nullptr == desktop) || desktop.isEmpty()) ? "" : desktop));
        }
    }

    return true;
}

ExportXlsx::ExportXlsx()
{
}

bool ExportXlsx::addVnuInfo(const QString &idx, const QString &name, const QString &number, const QString &desc,
                            const QString &time, const QString &url)
{
    static int ii = 0;

    if (ii == 0) {
        ++ii;
        mDoc.addSheet ("漏洞检查");
        mDoc.selectSheet ("漏洞检查");
        mDoc.write (1, 1, "序号");
        mDoc.write (1, 2, "漏洞名称");
        mDoc.write (1, 3, "补丁号");
        mDoc.write (1, 4, "补丁日期");
        mDoc.write (1, 5, "下载链接");
        mDoc.write (1, 6, "补丁描述");
        ++ii;
    }

    mDoc.selectSheet ("漏洞检查");
    mDoc.write (ii, 1, (((nullptr == idx) || idx.isEmpty()) ? "" : idx));
    mDoc.write (ii, 2, (((nullptr == name) || name.isEmpty()) ? "" : name));
    mDoc.write (ii, 3, (((nullptr == number) || number.isEmpty()) ? "" : number));
    mDoc.write (ii, 4, (((nullptr == time) || time.isEmpty()) ? "" : time));
    mDoc.write (ii, 5, (((nullptr == url) || url.isEmpty()) ? "" : url));
    mDoc.write (ii, 6, (((nullptr == desc) || desc.isEmpty()) ? "" : desc));
    ++ii;

    return true;
}

bool ExportXlsx::configInfo(const QString &idx, const QString &category, const QString &name, const QString &level,
                            const QString &checkMethod, const QString &repaireMethod, const QString &checkResult)
{
    static int ii = 0;

    if (ii == 0) {
        ++ii;
        mDoc.addSheet ("配置检查");
        mDoc.selectSheet ("配置检查");
        mDoc.write (1, 1, "序号");
        mDoc.write (1, 2, "类别");
        mDoc.write (1, 3, "配置检查名称");
        mDoc.write (1, 4, "等级");
        mDoc.write (1, 5, "检查方法");
        mDoc.write (1, 6, "修复建议");
        mDoc.write (1, 7, "检查结果");
        ++ii;
    }

    mDoc.selectSheet ("配置检查");
    mDoc.write (ii, 1, (((nullptr == idx) || idx.isEmpty()) ? "" : idx));
    mDoc.write (ii, 2, (((nullptr == category) || category.isEmpty()) ? "" : category));
    mDoc.write (ii, 3, (((nullptr == name) || name.isEmpty()) ? "" : name));
    mDoc.write (ii, 4, (((nullptr == level) || level.isEmpty()) ? "" : level));
    mDoc.write (ii, 5, (((nullptr == checkMethod) || checkMethod.isEmpty()) ? "" : checkMethod));
    mDoc.write (ii, 6, (((nullptr == repaireMethod) || repaireMethod.isEmpty()) ? "" : repaireMethod));
    mDoc.write (ii, 7, (((nullptr == checkResult) || checkResult.isEmpty()) ? "" : checkResult));
    ++ii;

    return true;
}

bool ExportXlsx::softwareInfo(const QString &idx, const QString &category, const QString &name, const QString &version,
                              const QString &installTime, const QString &installPath)
{
    static int ii = 0;

    if (ii == 0) {
        ++ii;
        mDoc.addSheet ("软件检查");
        mDoc.selectSheet ("软件检查");
        mDoc.write (1, 1, "序号");
        mDoc.write (1, 2, "软件分类");
        mDoc.write (1, 3, "软件名称");
        mDoc.write (1, 4, "软件版本");
        mDoc.write (1, 5, "安装时间");
        mDoc.write (1, 6, "安装路径");
        ++ii;
    }

    mDoc.selectSheet ("软件检查");
    mDoc.write (ii, 1, (((nullptr == idx) || idx.isEmpty()) ? "" : idx));
    mDoc.write (ii, 2, (((nullptr == category) || category.isEmpty()) ? "" : category));
    mDoc.write (ii, 3, (((nullptr == name) || name.isEmpty()) ? "" : name));
    mDoc.write (ii, 4, (((nullptr == version) || version.isEmpty()) ? "" : version));
    mDoc.write (ii, 5, (((nullptr == installTime) || installTime.isEmpty()) ? "" : installTime));
    mDoc.write (ii, 6, (((nullptr == installPath) || installPath.isEmpty()) ? "" : installPath));
    ++ii;

    return true;
}

void ExportXlsx::save(const QString &path)
{
    mDoc.saveAs (path);
}
