pragma auto_vacuum=0;
pragma default_cache_size=2000;
pragma encoding='UTF-8';
pragma page_size=1024;

/* Drop table [EstDlpFileAttribute] */
drop table if exists [EstDlpFileAttribute];

/* Table structure [EstDlpFileAttribute] */
CREATE TABLE [EstDlpFileAttribute] (
  [fileAttrId] integer NOT NULL PRIMARY KEY AUTOINCREMENT, 
  [filePath] varchar(512) NOT NULL ON CONFLICT REPLACE UNIQUE ON CONFLICT REPLACE);
CREATE UNIQUE INDEX [filePath] ON [EstDlpFileAttribute] ([fileAttrId]);


/* Drop table [EstDlpFilePolicyCheckRes] */
drop table if exists [EstDlpFilePolicyCheckRes];

/* Table structure [EstDlpFilePolicyCheckRes] */
CREATE TABLE [EstDlpFilePolicyCheckRes] (
  [fileChkResId] integer NOT NULL PRIMARY KEY AUTOINCREMENT, 
  [fileMd5] varchar(512) NOT NULL, 
  [policyID] varchar(128) NOT NULL, 
  [actionID] varchar(128),
  [context] VARCHAR(4096),
  [fileType] VARCHAR(128), 
  [fileSize] VARCHAR(128), 
  [isRecognized] VARCHAR(128));
CREATE UNIQUE INDEX [policyId] ON [EstDlpFilePolicyCheckRes] ([fileMd5], [policyID]);

/* Drop table [EstDlpFileRuleConn] */
drop table if exists [EstDlpFileRuleConn];

/* Table structure [EstDlpFileRuleConn] */
CREATE TABLE [EstDlpFileRuleConn] (
  [fileRuleConnId] integer NOT NULL PRIMARY KEY AUTOINCREMENT, 
  [checkRuleId] varchar(512) NOT NULL, 
  [checkResult] varchar(512), 
  [fileMd5] varchar(512) NOT NULL);
CREATE INDEX [checkRuleId] ON [EstDlpFileRuleConn] ([checkRuleId]);


/* Drop table [EstDlpEncryptFile] */
drop table if exists [EstDlpEncryptFile];

/* Table structure [EstDlpEncryptFile] */
CREATE TABLE [EstDlpEncryptFile] (
  [DlpEncFileId] integer NOT NULL PRIMARY KEY AUTOINCREMENT, 
  [DlpEncFilePath] varchar(512) NOT NULL ON CONFLICT REPLACE UNIQUE ON CONFLICT REPLACE);
CREATE UNIQUE INDEX [DlpEncFilePath] ON [EstDlpEncryptFile] ([DlpEncFileId]);

-- 中间数据
-- 扫描过程中 临时数据表(根据任务id 自建)
drop table if exists [scan_task_detail_taskid];
CREATE TABLE scan_task_detail_taskid (
    `detail_file_name_md5`              VARCHAR(256)                    NOT NULL,       -- 扫描的文件名
    `detail_file_name`                  TEXT                            NOT NULL,       -- 扫描文件路径
    `detail_file_status`                TINYINT         DEFAULT 0       NOT NULL,       -- 文件是否扫描：0 - 未扫描，1 - 扫描完成
    PRIMARY KEY(detail_file_name_md5)
);

-- 扫描结果，仅保存被扫描任务命中的文件
drop table if exists [scan_result];
CREATE TABLE scan_result (
    `ID`                                integer PRIMARY KEY AUTOINCREMENT ,             --  YYY 
    `scan_file_name`                    TEXT                            NOT NULL,       -- 文件绝对路径
    `policy_id`                         VARCHAR(32)                     NOT NULL,       -- 策略ID YYY
    `status`                            TINYINT         DEFAULT 0       NOT NULL,       -- 处理状态，0 - 未处理，5 - 删除，6 - 误报
    `scan_finished_time`                INTEGER         DEFAULT  0      NOT NULL,       -- 扫描完成时间
    `action_id`                         VARCHAR(32)                     NOT NULL,       -- 策略ID YYY
    `file_type`                         VARCHAR(32)                             ,       -- 文件类型 YYY 
    `file_size`                         VARCHAR(16)                     NOT NULL,       -- 文件大小 YYY
    `is_recognized`                     INTEGER         DEFAULT 1               ,       -- 文件是否识别 YYY
    `is_blocked`                        INTEGER         DEFAULT 1               ,       -- 文件是否识别 YYY
    `detect_result`                     TEXT                            NOT NULL,       -- 扫描结果
    `status_reported`                   TINYINT         DEFAULT 0       NOT NULL,       -- 是否已经上报处理状态，0 - 未上报处理状态，1:已经上报处理状态
    `change_time`                       INTEGER         DEFAULT 0               ,       -- 文件修改时间
    UNIQUE(scan_file_name, policy_id)                                                -- 联合唯一字段 YYY
);

-- 2022.11.14  added by YYY
-- 本地策略
-- 服务器绑定到本地的策略，以及任务中的策略
drop table if exists [local_policies];
CREATE TABLE local_policies (
    `ID`            integer PRIMARY KEY AUTOINCREMENT ,             --   
    `policy_id`     VARCHAR(32)                    NOT NULL UNIQUE,        --  来自服务器的策略ID
    `start_time`    INTEGER         DEFAULT  0      NOT NULL,       -- 扫描开始时间
    `finish_time`   INTEGER         DEFAULT  0      NOT NULL,       -- 扫描完成时间
    `refer_time`    INTEGER         DEFAULT  0      NOT NULL,       -- 最后扫描时间
    `status`        CHARACTER(1)    DEFAULT '0'     NOT NULL       -- 策略是否正常使用：0 - 正常使用，1 - 删除策略
);

-- 本地策略
-- 服务器绑定到本地的策略，以及任务中的策略
drop table if exists [not_match_files];
CREATE TABLE not_match_files (
    `I32_hash`         integer            ,                    --   
    `full_path`        text            NOT NULL UNIQUE,    --  未匹配文件全路径名 
    `policy_id_tms`    text                                --  (未匹配策略和策略ID对儿)的列表
);

drop table if exists [match_files];
CREATE TABLE match_files ( 
    `full_path`         TEXT                NOT NULL,                                   -- 匹配文件全路径名 
    `policy_id_tms`     TEXT,                                                           -- 匹配策略的<策略ID,文件修改时间>列表
    `file_type`         VARCHAR(32)         NOT NULL,                                   -- 文件类型
    `file_size`         VARCHAR(32),                                                    -- 文件大小
    `detect_result`     TEXT,                                                           -- 扫描结果
    `extends`           TEXT,                                                           -- 其它字段key1|value1{]key2|value2形式保存
    PRIMARY KEY (full_path)
);

CREATE TABLE scan_task (
    `task_id`                           VARCHAR(32)     DEFAULT 0       NOT NULL,       -- 任务 ID
    `task_name`                         TEXT            DEFAULT ''      NOT NULL,       -- 任务 
    `scan_interval`                     TINYINT         DEFAULT 4       NOT NULL,       -- 变动扫描定时时长(默认 4 小时)
    `scan_task_filter_name`             TEXT                            NOT NULL,       -- 扫描任务监测策略，以|分割
    `scan_task_dir`                     TEXT                            NOT NULL,       -- 扫描文件夹，以|分割
    `scan_task_dir_filterout`           TEXT                            NOT NULL,       -- 扫描例外文件夹，以|分割
    `scan_task_dir_filterout_fileext`   TEXT                            NOT NULL,       -- 扫描任务过滤的文件类型，以|分割
    `task_start_time`                   INTEGER         DEFAULT 0       NOT NULL,       -- 开始时间 
    `task_stop_time`                    INTEGER         DEFAULT 0       NOT NULL,       -- 结束时间 
    `task_file_count`                   INT4            DEFAULT 0       NOT NULL,       -- 终端总文件数
    `task_scan_file_count`              INT4            DEFAULT 0       NOT NULL,       -- 需要扫描的文件总数
    `task_scan_finished_file_count`     INT4            DEFAULT 0       NOT NULL,       -- 扫描完成的文件数
    `task_status`                       TINYINT         DEFAULT 0       NOT NULL,       -- 任务状态：0 - 未启动，1 - 进行中，2 - 停止，3 - 扫描结束 ; 4:暂停； 5:继续； 
    `scan_result_reuse`                 TINYINT         DEFAULT 0       NOT NULL,       -- 扫描结果复用：0 - 开启，1 - 关闭
    `scan_task_file_monitor`            TINYINT         DEFAULT 0       NOT NULL,       -- 变动文件监控：0 - 开启，1 - 关闭
    `scan_task_self_check`              TINYINT         DEFAULT 0       NOT NULL,       -- 自查任务：0 - 关闭，1 - 开启
    `exce_exte_file_type`                TEXT                            ,              -- 例外文件类型，以|分割
    PRIMARY KEY(task_id)
);

CREATE TABLE udisk_scan_result(
   sn integer PRIMARY KEY AUTOINCREMENT,
   match_id integer,
   org_name text,
   ip varchar(16),
   pc_name text not null,
   mac varchar(17),
   task_id text not null,
   task_name text not null,
   scan_path text,
   not_scan_path text,
   policy_ID text not null,
   policy_name text not null,
   event_ID text not null,
   file_name text not null,
   file_path text not null,
   scan_time integer,
   file_exte text not null,
   severity  varchar(16),
   os varchar(16),
   context text
);



